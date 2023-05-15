#include <utils/root.h>
#include <cpu/io.h>
#include <mm/memory.h>
#include "ne2k.h"

ne2k_t *ne2k;

int ne2k_in_byte(ne2k_t *this, int reg)
{
    int port = this->io_base + reg;
    return in_byte(port);
}

void ne2k_out_byte(ne2k_t *this, int reg, int value)
{
    int port = this->io_base + reg;
    out_byte(port, value);
}

void ne2k_out_word(ne2k_t *this, int reg0, int reg1, int value)
{
    int lo = value & 0xff;
    int hi = (value >> 8) & 0xff;
    ne2k_out_byte(this, reg0, lo);
    ne2k_out_byte(this, reg1, hi);
}

void ne2k_select_page0(ne2k_t *this)
{
    ne2k_out_byte(this, PG0_CR, CR_PS_PAGE0 | CR_NODMA | CR_STA);
}

void ne2k_select_page1(ne2k_t *this)
{
    ne2k_out_byte(this, PG0_CR, CR_PS_PAGE1 | CR_NODMA | CR_STA);
}

void ne2k_wait_rdc(ne2k_t *this)
{
    int retry = 100;
    while (retry--) {
        if (ne2k_in_byte(this, PG0_ISR) & ISR_RDC)
            break;
    }
    assert(retry > 0);
    ne2k_out_byte(this, PG0_ISR, ISR_RDC); /* ack the INT */
}

void ne2k_read_buffer(ne2k_t *this, int offset, void *buffer, int size)
{
    ne2k_out_word(this, PG0_RSAR0, PG0_RSAR1, offset);
    ne2k_out_word(this, PG0_RBCR0, PG0_RBCR1, size);
    ne2k_out_byte(this, PG0_CR, CR_STA|CR_READ);
    for (int i = 0; i < size; i++) {
        char *p = buffer + i;
        *p = ne2k_in_byte(this, PG0_DATA);
    }
    ne2k_wait_rdc(this);
}

void ne2k_read_smart(ne2k_t *this, int offset, void *buffer, int size)
{
    int rx_start = this->rx_start_page * DP8390_PAGE_SIZE;
    int rx_end = this->rx_end_page * DP8390_PAGE_SIZE;
    if (offset + size <= rx_end)
        ne2k_read_buffer(this, offset, buffer, size);

    int head_size = rx_end - offset;
    int tail_size = size - head_size;
    ne2k_read_buffer(this, offset, buffer, head_size);
    ne2k_read_buffer(this, rx_start, buffer + head_size, tail_size);
}

void ne2k_write_buffer(ne2k_t *this, int offset, void *buffer, int size)
{
    ne2k_out_word(this, PG0_RSAR0, PG0_RSAR1, offset);
    ne2k_out_word(this, PG0_RBCR0, PG0_RBCR1, size);
    ne2k_out_byte(this, PG0_CR, CR_STA|CR_WRITE);

    for (char *byte = buffer; byte < (char *)buffer + size; byte++)
        ne2k_out_byte(this, PG0_DATA, *byte);
    ne2k_wait_rdc(this);
}

static void ring_set_start(ne2k_t *this, int start)
{
    ne2k_out_byte(this, PG0_PSTART, start);
}

static void ring_set_end(ne2k_t *this, int start)
{
    ne2k_out_byte(this, PG0_PSTOP, start);
}

static int ring_get_in(ne2k_t *this)
{
    ne2k_select_page1(this);
    int in = ne2k_in_byte(this, PG1_CURR); 
    ne2k_select_page0(this);
    return in;
}

static void ring_set_in(ne2k_t *this, int in)
{
    ne2k_select_page1(this);
    ne2k_out_byte(this, PG1_CURR, in); 
    ne2k_select_page0(this);
}

static int ring_get_out(ne2k_t *this)
{
    int out = ne2k_in_byte(this, PG0_BNRY);
    return out;
}

static void ring_set_out(ne2k_t *this, int out)
{
    ne2k_out_byte(this, PG0_BNRY, out);
}

void ne2k_setup_buffer(ne2k_t *this)
{
    /* Setup Buffer */
    int buffer_start = 16 * 1024;
    int buffer_mid = round_up(buffer_start + ETH_MTU, DP8390_PAGE_SIZE);
    int buffer_end = 32 * 1024;
    this->tx_start_page = buffer_start / DP8390_PAGE_SIZE;
    this->tx_end_page = buffer_mid / DP8390_PAGE_SIZE; 
    this->rx_start_page = buffer_mid / DP8390_PAGE_SIZE;
    this->rx_end_page = buffer_end / DP8390_PAGE_SIZE;

    /* Setup Transmit Buffer */
    ne2k_out_byte(this, PG0_TPSR, this->tx_start_page);
    ne2k_out_word(this, PG0_TBCR0, PG0_TBCR1, 0);

    /* Setup Receive Buffer */
    ring_set_start(this, this->rx_start_page);
    ring_set_end(this, this->rx_end_page);
    ring_set_in(this, this->rx_start_page);
    ring_set_out(this, this->rx_start_page);
}

/* read NIC's physical address */
void ne2k_setup_mac(ne2k_t *this)
{
    unsigned char rom[ETH_ADDR_SIZE * 2];
    ne2k_read_buffer(this, 0, rom, sizeof(rom));

    for (int i = 0; i < ETH_ADDR_SIZE; i++) {
        int addr = rom[i * 2];
        this->hw_addr[i] = addr;
        ne2k_select_page1(this);
        ne2k_out_byte(this, PG1_PAR0 + i, addr);
        ne2k_select_page0(this);
    }
    memset(this->broadcast_addr, 0xFF, ETH_ADDR_SIZE);
    printf("ne2k found: io_base = %x, irq_no = %x, mac_addr = %s\n", 
           this->io_base, this->irq_no, eth_addr_to_string(this->hw_addr));
}

void ne2k_setup(ne2k_t *this, int io_base, int irq_no)
{
    this->io_base = io_base;
    this->irq_no = irq_no;

    /* Select Page 0 */
    ne2k_select_page0(this);

    /* Setup DCR/TCR/RCR */
    ne2k_out_byte(this, PG0_TCR, 0);
    ne2k_out_byte(this, PG0_RCR, RCR_AB); /* Accept Broadcast */

    int dcr = 0;
    dcr |= DCR_WTS_BYTE;            /* Byte-wide transfer */ 
    dcr |= DCR_BOS_LE;              /* Little endian for 80x86 */
    dcr |= DCR_LS_NORMAL;           /* Normal, not loopback */
    dcr |= DCR_FT1;
    ne2k_out_byte(this, PG0_DCR, dcr);

    ne2k_setup_buffer(this);
    ne2k_setup_mac(this);

    /* Setup IMR/ISR */
    ne2k_out_byte(this, PG0_IMR, IMR_ALL);
    ne2k_out_byte(this, PG0_ISR, 0xff); /* Ack the INT */
}

packet_t *ne2k_input(ne2k_t *this)
{
    int in = ring_get_in(this);
    int out = ring_get_out(this);
    assert(out != in);

    // 1. read head
    int head_offset = in * DP8390_PAGE_SIZE;
    int body_offset = head_offset + sizeof(dp8390_packet_t);
    dp8390_packet_t this_packet; 
    ne2k_read_buffer(this, head_offset, &this_packet, sizeof(dp8390_packet_t));

    // 2. read body
    int packet_size = this_packet.total_size - sizeof(dp8390_packet_t);
    packet_t *packet = packet_new(ETH_PAD, packet_size);
    ne2k_read_smart(this, body_offset, packet->data, packet_size);

    int next_packet = this_packet.next_packet;
    ring_set_out(this, next_packet);
    return packet;
}

int ne2k_output(ne2k_t *this, packet_t *packet)
{
    // RSAR/RBCR
    int offset = this->tx_start_page * DP8390_PAGE_SIZE;
    ne2k_write_buffer(this, offset, packet->data, packet->data_size);

    // TPSR/TBCR
    ne2k_out_byte(this, PG0_TPSR, this->tx_start_page);
    int data_size;
    if (packet->data_size < ETH_MIN_FRAME)
        data_size = ETH_MIN_FRAME;
    else
        data_size = packet->data_size;
    ne2k_out_word(this, PG0_TBCR0, PG0_TBCR1, data_size);

    ne2k_out_byte(this, PG0_CR, CR_NODMA|CR_TXP|CR_STA);
    return 0;
}

void ne2k_parse(ne2k_t *this)
{
    char data[256];
    int offset = ring_get_out(ne2k) * DP8390_PAGE_SIZE;
    ne2k_read_buffer(ne2k, offset, data, sizeof(data));
    dp8390_packet_t *dp = (void*)data;
    printf("TS:%d\n", dp->total_size);
    eth_head_dump((void*)data + sizeof(dp8390_packet_t));
}

void ne2k_isr(int irq_no)
{
    int status; 
    while (status = ne2k_in_byte(ne2k, PG0_ISR)) {
        if (status & ISR_PTX) {
            ne2k_out_byte(ne2k, PG0_ISR, ISR_PTX); // Ack the INT
            printf("Packet Transmited!\n");
            continue;
        }

        if (status & ISR_PRX) {
            ne2k_out_byte(ne2k, PG0_ISR, ISR_PRX); // Ack the INT
            printf("Packet Received!\n");
            continue;
        }

        assert(false);
    }
}

extern void arp_request(uint32_t ip_addr, nic_t *nic);

void ne2k_probe()
{
    ne2k = alloc_memory(sizeof(ne2k_t));
    nic_init((nic_t *)ne2k, "eth0", (nic_output_t)ne2k_output);
    install_nic((nic_t *)ne2k);

    ne2k_setup(ne2k, 0x280, 10);
    register_irq(10, ne2k_isr);
    enable_irq(10);
    sti();

    ne2k->ip_addr = inet_atoh("192.168.10.123");
    uint32_t ip_addr = inet_atoh("192.168.10.1");
    arp_request(ip_addr, (nic_t *)ne2k);
    arp_request(ip_addr, (nic_t *)ne2k);
    arp_request(ip_addr, (nic_t *)ne2k);
    for (;;);
}
