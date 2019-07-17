#include <wolfssl/options.h> 
#include <wolfssl/ssl.h>     

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAXLINE     4096
#define LISTENQ     1024
#define SERV_PORT   11111

/*
 * Identify which psk key to use.
 */
static unsigned int myServerPSK(WOLFSSL* ssl, const char* identity,
                           unsigned char* key, unsigned int key_max_len)
{
    (void)ssl;
    (void)key_max_len;

    if (strncmp(identity, "Client", 15) != 0) {
        return 0;
    }

    key[0] = 26;
    key[1] = 43;
    key[2] = 60;
    key[3] = 77;

    return 4;
}

int main()
{
    int  n;              
    int                 listenfd, connfd;
    int                 opt;
    char                buff[MAXLINE];
    char buf[MAXLINE];   /* buffer */
    char response[] = "Welcome Field Device";
    struct sockaddr_in  cliAddr, servAddr;
    socklen_t           cliLen;
    WOLFSSL_CTX*         ctx;

    wolfSSL_Init();

  
    if ((ctx = wolfSSL_CTX_new(wolfSSLv23_server_method())) == NULL) {
        printf("Fatal error : wolfSSL_CTX_new error\n");
        return 1;
    }

   
    wolfSSL_CTX_set_psk_server_callback(ctx, myServerPSK);
    wolfSSL_CTX_use_psk_identity_hint(ctx, "wolfssl server");
    if (wolfSSL_CTX_set_cipher_list(ctx, "PSK-AES128-CBC-SHA256")
                                   != SSL_SUCCESS) {
        printf("error : can't set cipher\n");
        return 1;
    }


 
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port        = htons(SERV_PORT);

    /* create a socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("error : socket creation error\n");
        return 1;
    }

  
    opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt,
               sizeof(int)) != 0) {
        printf("error : socket binding error error\n");
        return 1;
    }
    if (bind(listenfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        printf("error : socket binding error2\n");
        return 1;
    }

   
    if (listen(listenfd, LISTENQ) < 0) {
        printf("Fatal error : cant listen the socket\n");
        return 1;
    }

    /* wait field devices to connect */
    for ( ; ; ) {
        WOLFSSL* ssl;

        cliLen = sizeof(cliAddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliAddr, &cliLen);
        if (connfd < 0) {
            printf(" error : cannot accept the client error\n");
            return 1;
        }
        else {
            printf("Connection from %s, port %d\n",
                   inet_ntop(AF_INET, &cliAddr.sin_addr, buff, sizeof(buff)),
                   ntohs(cliAddr.sin_port));

            if ((ssl = wolfSSL_new(ctx)) == NULL) {
                printf("Wolfssl Context error. It is probably because of cipher settings\n");
                return 1;
            }
            
            
            wolfSSL_set_fd(ssl, connfd);
            
            /* set buffer empty*/
            memset(buf, 0, MAXLINE);
            
            /* reads data */
            n = wolfSSL_read(ssl, buf, MAXLINE);
            if (n > 0) {
                printf("%s\n", buf);
   
                if (wolfSSL_write(ssl, response, strlen(response)) > strlen(response)) {
                    printf(" error : wring error\n");
                    return 1;
                }
            }
            if (n < 0) {
                printf("reading error\n");
                return 1;
            }

          
            wolfSSL_shutdown(ssl);
            wolfSSL_free(ssl);

            if (close(connfd) == -1) {
                printf("Fatal error : close error\n");
                return 1;
            }
        }
    }
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    return 0;
}
