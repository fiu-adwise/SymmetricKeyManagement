
#include <wolfssl/options.h> /* included for options sync */
#include <wolfssl/ssl.h>     /* must include this to use wolfSSL security */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>

#define     MAXLINE 256      /* max text line length */
#define     SERV_PORT 11111  /* default port*/

/*
 *psk client set up.
 */
static inline unsigned int MySymetricKeyManagement(WOLFSSL* ssl, const char* hint,
        char* identity, unsigned int id_max_len, unsigned char* key,
        unsigned int key_max_len)
{
    (void)ssl;
    (void)hint;
    (void)key_max_len;

    strncpy(identity, "MyClient", id_max_len);
    
    key[0] = 26;
    key[1] = 43;
    key[2] = 60;
    key[3] = 77;

    return 4;
}

int main(int argc, char **argv)
{
    int ret, sockfd;
    char sendMessage[MAXLINE]="Hello Utility I am a fieldDevice"; 
    char recvMessage[MAXLINE];
    WOLFSSL* ssl;
    WOLFSSL_CTX* ctx;
    struct sockaddr_in servaddr;;

    if (argc != 2) {
        printf("Usage: please provide IP address <IPaddress>\n");
        return 1;
    }

    wolfSSL_Init(); 

   
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        fprintf(stderr, "Wolfssl Error.\n");
        return 1;
    }

    /* create a socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* allocate Memory */
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    /* convert IP address */
    ret = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    if (ret != 1) {
        printf("convertion error\n");
	    return 1;
    }

    /* setup shared keys */
    wolfSSL_CTX_set_psk_client_callback(ctx, My_Psk_Client_Cb);

    /* connection on a socket */
    ret = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    if (ret != 0) {
        printf("Error\n");
        return 1;
    }

    /* creat object after each connection */
    if ( (ssl = wolfSSL_new(ctx)) == NULL) {
        fprintf(stderr, "object error.\n");
        return 1;
    }

    /* assign a file descriptor */
    ret = wolfSSL_set_fd(ssl, sockfd);

    if (ret != SSL_SUCCESS) {
        return 1;
    }

    /* send message to the server/ utility */
	if (wolfSSL_write(ssl, sendMessage, MAXLINE) != sizeof(sendline)) {
	    printf("Writing error\n");
	    return 1;
    }
    
    /* check if server ended connected  */
    if (wolfSSL_read(ssl, recvMessage, MAXLINE) < 0 ) {
        printf("FieldDevice: Server Terminated!\n");
        return 1;
    }

	printf("Server Message: %s\n", recvline);

    /* cleanup */
    wolfSSL_free(ssl);
    
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

 
    return ret;
}
