#ifndef _DP8390_H
#define _DP8390_H

// http://wiki.osdev.org/DP8390

typedef struct {
    uint8_t status;
    uint8_t next_packet;
    uint16_t total_size;
} dp8390_packet_t;

#define DP8390_PAGE_SIZE 256

/* Page 0 Register, Read */
#define PG0_CR      0x00    /* Command Register */
#define PG0_CLDA0   0x01  
#define PG0_CLDA1   0x02  
#define PG0_BNRY    0x03    /* Boundary Page */
#define PG0_TSR     0x04    /* Transmit Status Register */
#define PG0_NCR     0x05
#define PG0_FIFO    0x06
#define PG0_ISR     0x07    /* Interrupt Status Register */
#define PG0_CRDA0   0x08
#define PG0_CRDA1   0x09
// reserve
// reserve
#define PG0_RSR     0x0c    /* Receive Status Register */
#define PG0_CNTR0   0x0d
#define PG0_CNTR1   0x0e
#define PG0_CNT2    0x0f
#define PG0_DATA    0x10

/* Page 0 Register, Write */
// Command Register
#define PG0_PSTART  0x01    /* Page Start */
#define PG0_PSTOP   0x02    /* Page Stop */
// PG0_BNRY
#define PG0_TPSR    0x04    /* Transmit Page Start Register */
#define PG0_TBCR0   0x05    /* Transmit Byte Count Register 0 */ 
#define PG0_TBCR1   0x06    /* Transmit Byte Count Register 1 */ 
// PG0_ISR
#define PG0_RSAR0   0x08    /* Remote Start Address Register 0 */
#define PG0_RSAR1   0x09    /* Remote Start Address Register 1 */             
#define PG0_RBCR0   0x0a    /* Remote Byte Count Register 0 */
#define PG0_RBCR1   0x0b    /* Remote Byte Count Register 0 */
#define PG0_RCR     0x0c    /* Receive Config Register */
#define PG0_TCR     0x0d    /* Transmit Config Register */
#define PG0_DCR     0x0e    /* Data Config Register */
#define PG0_IMR     0x0f    /* Interrrupt Master Register */
// PG0_DATA

/* Page 1 Register, Read */
#define PG1_CR      0x00    /* Command Register */
#define PG1_PAR0    0x01    /* Physical Address Register */
#define PG1_CURR    0x07    /* Current Page */
#define PG1_MAR0    0x08    

/* Command register */
#define CR_STP      0x01    /* Stop */
#define CR_STA      0x02    /* Start */
#define CR_TXP      0x04    /* Transmit Packet */
#define CR_READ     0x08    /* Read Buffer */
#define CR_WRITE    0x10    /* Write Buffer */ 
#define CR_NODMA    0x20    /* ??? */ 
#define CR_PS0      0x40    /* Page Select 0 */
#define CR_PS1      0x80    /* Page Select 1 */
#define CR_PS_PAGE0 0x00
#define CR_PS_PAGE1 CR_PS0
#define CR_PS_PAGE2 (CR_PS1 | CR_PS0)

/* Interrupt Status Register */
#define ISR_PRX     0x01    /* Packet Receive */
#define ISR_PTX     0x02    /* Packet Transmit */
#define ISR_RXE     0x04    /* Receive Error */
#define ISR_TXE     0x08    /* Transmit Error */
#define ISR_ROVW    0x10
#define ISR_CNT     0x20
#define ISR_RDC     0x40    /* Remote DMA Complete */
#define ISR_RST     0x80

/* Interrupt Mask Register */
#define IMR_PRXE    0x01    /* Packet Receive Interrupt Enable */
#define IMR_PTXE    0x02    /* Packet Transmit Interrupt Enable */
#define IMR_RXEE    0x04    /* Receive Error Enable */
#define IMR_TXEE    0x08    /* Transmit Error Enable */
#define IMR_ROVWE   0x10
#define IMR_CNTE    0x20
/* Enable INT when bit is on */
#define IMR_ALL     0x3f
#define IMR_RDCE    0x40    /* Remote DMA Complete */
#define IMR_XXX     0x80

/* Data Config Register */
#define DCR_WTS     0x01    /* Word Transfer Mode */
#define DCR_WTS_BYTE  0x00  /* Byte */
#define DCR_WTS_WORD  0x01  /* Word */

#define DCR_BOS     0x02    /* Byte Order Selection */
#define DCR_BOS_LE    0x00  /* Little Endian */
#define DCR_BOS_BE    0x02  /* Big Endian */

#define DCR_LAS     0x04    /* Long Address Select */

#define DCR_LS      0x08    /* Loopback Selection */
#define DCR_LS_LOOP   0x00  /* Loopback mode*/
#define DCR_LS_NORMAL 0x08  /* Normal mode */

#define DCR_ARM     0x10    /* Auto-Initailize Remote */
#define DCR_FT0     0x20    /* Fifo Threadhold 0 */
#define DCR_FT1     0x40    /* Fifo Threadhold 1 */
#define DCR_XXX     0x80

/* Transmit Config Register */
#define TCR_CRC     0x01
#define TCR_LB0     0x02
#define TCR_LB1     0x04
#define TCR_ATD     0x08
#define TCR_OFST    0x10

/* Receive Config Register */
#define RCR_SEP     0x01
#define RCR_AR      0x02
#define RCR_AB      0x04    /* Accept Broadcast */
#define RCR_AM      0x08
#define RCR_PRO     0x10
#define RCR_MON     0x20

/* Transmit Status Register */
#define TSR_PTX     0x01
#define TSR_XXX     0x02
#define TSR_COL     0x04
#define TSR_ABT     0x08
#define TSR_CRS     0x10
#define TSR_FU      0x20
#define TSR_CDH     0x40
#define TSR_OWC     0x80

/* Receive Status Register */
#define RSR_PRX     0x01
#define RSR_CRC     0x02
#define RSR_FAE     0x04
#define RSR_FO      0x08
#define RSR_MPA     0x10
#define RSR_PHY     0x20
#define RSR_DIS     0x40
#define RSR_DFR     0x80

#endif
