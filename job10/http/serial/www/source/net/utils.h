#ifndef _NET_UTILS_H
#define _NET_UTILS_H

#define LITTLE_ENDIAN   123
#define BIG_ENDIAN      456
#define CPU_ENDIAN      LITTLE_ENDIAN

static inline uint16_t htons(uint16_t host)
{
#if CPU_ENDIAN == LITTLE_ENDIAN
    uint32_t a = host & 0xff;
    uint32_t b = host >> 8;
    return (a << 8) + b;
#else
    return host;
#endif
}

static inline uint32_t htonl(uint32_t host)
{
#if CPU_ENDIAN == LITTLE_ENDIAN
    uint32_t a = host & 0xff;
    uint32_t b = (host >> 8) & 0xff;
    uint32_t c = (host >> 16) & 0xff;
    uint32_t d = host >> 24;
    return (a << 24) + (b << 16) + (c << 8) + d;
#else
    return host;
#endif
}

static inline uint16_t ntohs(uint16_t net)
{
    return htons(net);
}

static inline uint32_t ntohl(uint32_t net)
{
    return htonl(net);
}

static inline uint16_t decode_uint16(uint8_t *n)
{
    uint16_t host;
    uint8_t *h = (uint8_t *)&host;

#if CPU_ENDIAN == LITTLE_ENDIAN
    h[0] = n[1];
    h[1] = n[0];
#else
    h[0] = n[0];
    h[1] = n[1];
#endif

    return host;
}

static inline uint32_t decode_uint32(uint8_t *n)
{
    uint16_t host;
    uint8_t *h = (uint8_t *)&host;

#if CPU_ENDIAN == LITTLE_ENDIAN
    h[0] = n[3];
    h[1] = n[2];
    h[2] = n[1];
    h[3] = n[0];
#else
    h[0] = n[0];
    h[1] = n[1];
    h[2] = n[2];
    h[3] = n[3];
#endif

    return host;
}

static inline void encode_uint16(uint8_t *n, uint16_t host)
{
    uint8_t *h = (uint8_t *)&host;

#if CPU_ENDIAN == LITTLE_ENDIAN
    n[0] = h[1];
    n[1] = h[0];
#else
    n[0] = h[0];
    n[1] = h[1];
#endif
}

static inline void encode_uint32(uint8_t *n, uint32_t host)
{
    uint8_t *h = (uint8_t *)&host;

#if CPU_ENDIAN == LITTLE_ENDIAN
    n[0] = h[3];
    n[1] = h[2];
    n[2] = h[1];
    n[3] = h[0];
#else
    n[0] = h[0];
    n[1] = h[1];
    n[2] = h[2];
    n[3] = h[3];
#endif
}

extern char *inet_ntoa(uint32_t net);
extern uint32_t inet_aton(char *string);

static inline char *inet_htoa(uint32_t host)
{
    uint32_t net = htonl(host);
    return inet_ntoa(net);
}

static inline uint32_t inet_atoh(char *string)
{
    uint32_t net = inet_aton(string);
    uint32_t host = ntohl(net);
    return host;
}
#endif
