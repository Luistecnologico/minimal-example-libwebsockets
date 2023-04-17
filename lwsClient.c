#include <libwebsockets.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int callback_websocket(struct lws *wsi,
enum lws_callback_reasons reason,
void *user,
void *in,
size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            printf("Conexión establecida con el servidor\n");
            char *message = "Hola, servidor!";
            size_t message_len = strlen(message);
            unsigned char *buffer = malloc(LWS_SEND_BUFFER_PRE_PADDING + message_len + LWS_SEND_BUFFER_POST_PADDING);
            memcpy(buffer + LWS_SEND_BUFFER_PRE_PADDING, message, message_len);
            int write_len = lws_write(wsi, buffer + LWS_SEND_BUFFER_PRE_PADDING, message_len, LWS_WRITE_TEXT);
            free(buffer);
            if (write_len < message_len) {
                printf("Error al enviar mensaje al servidor\n");
                return -1;
            }
            printf("Mensaje enviado al servidor: %s\n", message);

            break;
        
        case LWS_CALLBACK_CLIENT_RECEIVE:
            printf("Mensaje recibido del servidor: %s\n", (char *)in);
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            char *message = "Hola, servidor!";
            size_t message_len = strlen(message);
            unsigned char *buffer = malloc(LWS_SEND_BUFFER_PRE_PADDING + message_len + LWS_SEND_BUFFER_POST_PADDING);
            memcpy(buffer + LWS_SEND_BUFFER_PRE_PADDING, message, message_len);
            int write_len = lws_write(wsi, buffer + LWS_SEND_BUFFER_PRE_PADDING, message_len, LWS_WRITE_TEXT);
            free(buffer);
            if (write_len < message_len) {
                printf("Error al enviar mensaje al servidor\n");
                return -1;
            }
            printf("Mensaje enviado al servidor: %s\n", message);
            break;
        }

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            printf("Error al conectar con el servidor\n");
            break;
        
        default:
            break;
    }

    return 0;
}

int main(void)
{
    struct lws_protocols protocols[] = {
        { "ws-only", callback_websocket, 0, 0},
        { NULL, NULL, 0, 0}
    };
    struct lws_context_creation_info info = {
        .port = CONTEXT_PORT_NO_LISTEN,
        .protocols = protocols,
        .iface = NULL,
        .user = NULL,
        .ka_time = 0,
        .ka_probes = 0,
        .ka_interval = 0,
    };

    struct lws_context *context = lws_create_context(&info);

    if (!context) {
        printf("Error al crear el contexto\n");
        return -1;
    }

    struct lws_client_connect_info connect_info = {
        .context = context,
        .address = "localhost",
        .port = 9000,
        .path = "/websocket",
        .host = NULL,
        .origin = NULL,
        .protocol = "ws-only",
        .pwsi = NULL,
        .ssl_connection = 0,
    };

    struct lws *wsi = lws_client_connect_via_info(&connect_info);

    if (!wsi) {
        printf("Error al conectar con el servidor\n");
        lws_context_destroy(context);
        return -1;
    }

    while (1) {
        lws_service(context, 50);
    }

    lws_context_destroy(context);

    return 0;
}