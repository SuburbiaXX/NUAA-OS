#include "std.h"
#include "http.h"

char *web_root = "www";// 根目录

// http_write_chunk(fw, "hello", 5)
// 共发送 3+5+2 个字符
// 5\r\nHELLO\r\n
void http_write_chunk(FILE *fw, void *chunk, int size) {// http 块
    fprintf(fw, "%x\r\n", size);
    fwrite(chunk, size, 1, fw);
    fprintf(fw, "\r\n");
}

// 共发送 3+0+2 个字符
// 0\r\n\r\n
void http_end(FILE *fw) {// http 尾
    http_write_chunk(fw, NULL, 0);
    fflush(fw);
}

void http_prints(FILE *fw, void *string) {// 以字符串的形式输出一行 html 字符串
    int size = strlen(string);
    http_write_chunk(fw, string, size);
}

void http_printf(FILE *fw, const char *format, ...) {// 以某种格式化的形式输出一行 html 字符串
    va_list ap;
    va_start(ap, format);
    char chunk[100];
    int size = vsprintf(chunk, format, ap);
    va_end(ap);

    http_write_chunk(fw, chunk, size);
}

void http_send_headers(FILE *fp, char *content_type) {// 发送 http 头
    fprintf(fp, "HTTP/1.1 200 OK\r\n");
    fprintf(fp, "Server: tiny httpd\r\n");
    fprintf(fp, "Content-type: %s\r\n", content_type);
    fprintf(fp, "Transfer-Encoding: chunked\r\n");
    fprintf(fp, "\r\n");
}

// GET /index.html HTTP/1.1\r\n
char *http_parse_req(FILE *fr, char *req, int req_size) {// 解析请求, 获取 http 请求的路径
    fgets(req, req_size, fr); 
    puts(req);
    char *http_path = strtok(req, " ");// 获取了 GET
    http_path = strtok(NULL, " ");// 获取了路径, 可能为空, 这里要小心
    return http_path;
}

void http_send_dir(char *dir_path, FILE *fw) {// 发送请求的文件夹中的文件列表
    // 获取文件夹中的文件列表
    DIR *dir = opendir(dir_path);
    // 此处是以 index.html 的所在位置为根, 即以 www 文件夹为根
    // 构造的 dir_path 目录中包涵 web_root, 所以需要去掉 web_root, 得到当前路径
    char *cur_path = strchr(dir_path, '/');

    // 获取并构造当前路径的父路径, 获取第一个 / 和倒数第二个 / 之间的内容
    char *first = strchr(cur_path, '/'), *last = strrchr(cur_path, '/'), *father_path;
    if(first != last) {// 如果不是根目录, 则构造父路径
        father_path = strncpy(malloc(last - first + 1), first, last - first);
        father_path[last - first] = '\0';
    } else
        father_path = "/";

    struct dirent *entry;
    struct stat file_stat;
    char file_path[256];

    http_send_headers(fw, "text/html");// http 头
    http_printf(fw, "<h1>Index of %s</h1>\n", cur_path);
    http_printf(fw, "<hr>\n");// 隔断线
    http_printf(fw, "<ol>\n");// 列表
    http_printf(fw, "<a href=\"%s\">../</a><br>\n", father_path);// 添加父路径
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 | strcmp(entry->d_name, "..") == 0)// 跳过 . 和 ..
            continue;

        // 拼接出文件的路径
        // 本想用 sprintf, 但是之中要添加 / 编译会有警告, 所以用 strcat 和 strcpy
        strcat(strcpy(file_path, dir_path), "/");
        strcat(file_path, entry->d_name);
        
        // 判断是文件夹还是文件
        stat(file_path, &file_stat);
        if (S_ISDIR(file_stat.st_mode))// 如果是文件夹, 则在文件名后面加上 /
            http_printf(fw, "<a href=\"%s/%s\">%s/</a><br>\n", cur_path, entry->d_name, entry->d_name);
        else// 如果是文件, 则直接使用文件名
            http_printf(fw, "<a href=\"%s/%s\">%s</a><br>\n", cur_path, entry->d_name, entry->d_name);
    }
    http_printf(fw, "</ol>\n");
    http_printf(fw, "<hr>\n");
    http_end(fw);// http 尾
    closedir(dir);
}

void http_send_file(char *file_path, FILE *fw) {// 发送请求的文件内容
    FILE *file = fopen(file_path, "r");
    // 如果需要打开的文件不存在，返回404 Not Found
    if (file == NULL) {
        http_send_headers(fw, "text/html");
        http_prints(fw, "<h1>404 Not Found</h1>");
        http_end(fw);
        return;
    }

    // 区分是 html 文件还是其他文件, 设置 content_type, 以便浏览器正确解析渲染
    // text/html 是 html 格式
    // text/plain 是纯文本格式
    char *content_type;
    if (strstr(file_path, ".html") != NULL)
        content_type = "text/html";
    else
        content_type = "text/plain";

    http_send_headers(fw, content_type);// http 头

    // 获取文件长度
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // 发送 file_path 的内容, 以一个 chunk 的形式发送
    char *file_content = malloc(file_size);
    fread(file_content, file_size, 1, file);
    http_write_chunk(fw, file_content, file_size);
    free(file_content);

    http_end(fw);// http 尾
    fclose(file);
}

void http_handler(int fd) {
    FILE *fr = fdopen(fd, "r");
    FILE *fw = fdopen(fd, "w");

    char req[1024];
    char *http_path;
    http_path = http_parse_req(fr, req, sizeof(req));// 解析请求, 获取 http 请求的路径
    if(http_path != NULL) {// 防止直接关掉浏览器的时候出现段错误, 此时 http_path 为 NULL
        if (strcmp(http_path, "/") == 0) {// 如果请求路径为根目录 / , 则将路径设置为 index.html
            http_path = "/index.html";
        }

        // 使用相对路径, 拼接出访问文件路径
        char file_path[256];
        sprintf(file_path, "%s%s", web_root, http_path);

        if (strstr(http_path, "/app") != NULL) {// 判断是否访问 /app 下, 通过read_pipe的方式执行命令
            pid_t pid;
            int fd[2];
            pipe(fd);
            pid = fork();
            if(pid == 0) {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);

                chdir("./www/app/");// 更改当前工作目录
                char *argv[2], *envv[2] = {NULL, NULL};
                char *p, *q;
                char temp[256];
                p = strtok(http_path, "?");
                q = strrchr(p, '/');
                // printf("q: %s\r\n", q); // q 获得了 ? 之前的内容 /xxx

                p = strtok(NULL, "?");
                // printf("p: %s\r\n", p); // p 获得了 ? 之后的内容 xxx=xxx&xxx=xxx

                if (p != NULL) {
                    strncpy(temp, p, sizeof(temp) - 1);
                    temp[sizeof(temp) - 1] = '\0';
                    envv[0] = malloc(strlen("QUERY_STRING=") + strlen(temp) + 1);
                    strcpy(envv[0], "QUERY_STRING=");
                    strcat(envv[0], temp);
                }
                // printf("envv[0]: %s\r\n", envv[0]); // envv[0] 获得了 QUERY_STRING=xxx=xxx&xxx=xxx

                argv[0] = malloc(strlen(q) + 3);
                sprintf(argv[0], ".%s", q);
                argv[1] = NULL;
                envv[1] = NULL;
                execve(argv[0], argv, envv);
            }
            wait(NULL);
            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
            http_send_headers(fw, "text/html");

            char buf[65535];
            memset(buf, 0, sizeof(buf));// 防止指令直接的输出相互干扰, 要重置 buf
            while(read(0, buf, 65535)) {
                http_prints(fw, buf);
            }
            http_end(fw);
        }else{
            // 判断请求的是文件夹还是文件
            struct stat file_stat;
            stat(file_path, &file_stat);
            if (S_ISDIR(file_stat.st_mode))// 如果是文件夹, 则列出文件夹中的文件, 整合成一个 html 格式的文件列表发送
                http_send_dir(file_path, fw);
            else// 如果是文件, 则发送文件内容
                http_send_file(file_path, fw);
        }
    }
}