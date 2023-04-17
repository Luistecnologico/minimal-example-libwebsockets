#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>

#define PORT 9000
#define BUF_SIZE 512

static int listenFD = -1;

static int callback_http(struct lws *wsi, 
enum lws_callback_reasons reason,
void *user,
void *in,
size_t len) {
    return 0;
}

static int callback_ws(struct lws *wsi, 
enum lws_callback_reasons reason,
void *user,
void *in,
size_t len) {   
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            // Por cada nueva conexion se crea un nuefo fd.
            // Por lo que no se puede coger el fd inicial
            printf("Connection established\n");
            listenFD = lws_get_socket_fd(wsi);
            printf("File description: %d\n", listenFD);
            break;

        case LWS_CALLBACK_RECEIVE:
            char buf[BUF_SIZE];
            memcpy(buf, in, len);
            buf[len] = '\0';
            printf("Received data: %s\n", buf);

            // Echo the received message back to the client
            lws_write(wsi, (unsigned char *)buf, len, LWS_WRITE_TEXT);
            break;

        case LWS_CALLBACK_CLOSED:
            printf("Connection closed\n");
            break;

        default:
            break;
    }
    return 0;
}

int main(int argc, char **argv)
{
    //struct lws_conext *context;
    struct lws_context_creation_info info;
    struct lws_protocols protocols[] = {
        { "http-only", callback_http, 0, 0},
        { "ws-only", callback_ws, 0, 0},
        { NULL, NULL, 0, 0}
    };
    
    int n = 0;

    memset(&info, 0, sizeof(info));
    info.port = PORT;
    info.protocols = protocols;

    struct lws_context *context = lws_create_context( &info );
    if (!context) {
        printf("Failed to create context\n");
        return -1;
    }

    printf("WebSocket server listening on port %d...\n", PORT);

    while (n >= 0) {
        n = lws_service(context, 50);
    }

    lws_context_destroy(context);

    return 0;
}