#ifndef _HTTP_H
#define _HTTP_H

extern int debug_mode;
extern void http_handler(int fd);
extern void http_send_headers(FILE *fp, char *content_type);
extern void http_write_chunk(FILE *fw, void *chunk, int size);

#endif
