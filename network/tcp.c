#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>


#define BUF_SIZE 8192

int main (int argc, char *argv[]){
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <knguyenquang@umass.edu> <port> <host_ip>\n", argv[0]);
        return 1;
    }

    char *email = argv[1];
    int port = atoi(argv[2]);
    char *host = argv[3];

    //open TCP socket 
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Error creating socket\n");
        return 1;
    }

    // connect to server 
    struct sockaddr_in addr; 
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n");
        close(sock);
        return 1;
    }
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Connection failed\n");
        close(sock);
        return 1;
    }

    // send HELLO 
    char hello[256];
    int hlen = snprintf(hello, sizeof(hello), "cs230 HELLO %s\n", email);
    if (send(sock, hello, hlen, 0) < 0) {
        fprintf(stderr, "Error sending HELLO\n");
        close(sock);
        return 1;
    }

    // read task from server 
    char buf[BUF_SIZE];
    size_t buf_len = 0; 
    while(1){
        // read status message 
        char *newline; 
        while((newline = memchr(buf, '\n', buf_len)) == NULL){ 
            ssize_t n = recv(sock, buf + buf_len, sizeof(buf) - buf_len, 0);
            if (n <= 0) {
                fprintf(stderr, "Error reading from server\n");
                close(sock);
                return 1;     
            }
            buf_len += n;
        }

        // extract message
        size_t line_len = newline - buf;
        char line[BUF_SIZE];
        memcpy(line, buf, line_len);
        line[line_len] = '\0';
        // shift remaining data to front of buffer
        memmove(buf, newline + 1, buf_len - line_len - 1);
        buf_len -= (line_len + 1);

        printf("Received: %s\n", line);

        //check for BYE 
        if (strstr(line, " BYE") != NULL) {
            printf("\n Captured the Flag\n%s\n", line);
            break;
        }

        // parse task
        char op[16];
        unsigned int num;
        if(sscanf(line, "cs230 STATUS %15s %u", op, &num) != 2) {
            fprintf(stderr, "Invalid task format\n");
            close(sock);
            return 1;
        }

        // perform task 
        unsigned int result; 
        if (strcmp(op, "LEFT")==0){
            result = num<<(num%10);
        } else if (strcmp(op, "RIGHT")==0){
            result = num>>(num%10);
        } else if (strcmp(op, "OR")==0){
            result = num | ((num&0xF)*0x11111111U);
        } else if (strcmp(op, "AND")==0){
            result = num & ((num&0xF)*0x11111111U);
        }else {
            fprintf(stderr, "Unknown operation\n");
            close(sock);
            return 1;
        }

        char response[256];
        int rlent = snprintf(response, sizeof(response), "cs230 %u\n", result);
        printf("Send: %s", response);
        if (send(sock, response, rlent, 0) < 0) {
            fprintf(stderr, "Error sending response\n");
            close(sock);
            return 1;
        }
    }
    close(sock);
    return 0;
}