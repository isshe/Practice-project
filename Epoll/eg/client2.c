#include <sys/types.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <err.h>
#include <event.h>
#include <evhttp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>

#define BUFSIZE 4096
#define SLEEP_MS 10

char buf[BUFSIZE];

int bytes_recvd = 0;
int chunks_recvd = 0;
int closed = 0;
int connected = 0;

static char ip_array[300] =  "192.168.240.23,192.168.240.24,192.168.240.25,192.168.240.26,192.168.240.27,192.168.240.28,192.168.240.29,192.168.240.30,192.168.240.31,192.168.240.32";
static char server_ip[16] =  "192.168.240.22";
static int server_port = 8000;
static int max_conns = 62000;

// called per chunk received
void chunkcb(struct evhttp_request *req, void *arg) {
    int s = evbuffer_remove( req->input_buffer, &buf, BUFSIZE );
    bytes_recvd += s;
    chunks_recvd++;
    if (connected >= max_conns && chunks_recvd % 10000 == 0)
        printf(">Chunks: %d\tBytes: %d\tClosed: %d\n", chunks_recvd, bytes_recvd, closed);
}

// gets called when request completes
void reqcb(struct evhttp_request *req, void *arg) {
    closed++;
}

int main(int argc, char **argv) {
    int ch;
    while ((ch = getopt(argc, argv, "o:h:p:m:")) != -1) {
        switch (ch) {
        case 'h':
            printf("host is %s\n", optarg);
            strncpy(server_ip, optarg, 15);
            break;
        case 'p':
            printf("port is %s\n", optarg);
            server_port = atoi(optarg);
            /*strncpy(server_ip, optarg, 15);*/
            break;
        case 'm':
            printf("max_conns is %s\n", optarg);
            max_conns = atoi(optarg);
            /*strncpy(server_ip, optarg, 15);*/
            break;
        case 'o':
            printf("ori_ips is %s\n", optarg);

            strncpy(ip_array, optarg, 300 - 1);
            break;
        }
    }

    event_init();
    struct evhttp *evhttp_connection;
    struct evhttp_request *evhttp_request;
    char path[32];
    int i;

    char delims[] = ",";
    char *ori_ip = NULL;
    ori_ip = strtok( ip_array, delims );
    while (ori_ip != NULL) {
        for (i = 1; i <= max_conns; i++) {
            evhttp_connection = evhttp_connection_new(server_ip, server_port);
            evhttp_connection_set_local_address(evhttp_connection, ori_ip);
            evhttp_set_timeout(evhttp_connection, 864000); // 10 day timeout
            evhttp_request = evhttp_request_new(reqcb, NULL);
            evhttp_request->chunk_cb = chunkcb;
            sprintf(&path, "/test/%d", ++connected);

            if (i % 1000 == 0)
                printf("Req: %s\t->\t%s\n", ori_ip, &path);

            evhttp_make_request( evhttp_connection, evhttp_request, EVHTTP_REQ_GET, path );
            evhttp_connection_set_timeout(evhttp_request->evcon, 864000);
            event_loop( EVLOOP_NONBLOCK );

            if ( connected % 1000 == 0 )
                printf("\nChunks: %d\tBytes: %d\tClosed: %d\n", chunks_recvd, bytes_recvd, closed);

            usleep(SLEEP_MS * 10);
        }

        ori_ip = strtok( NULL, delims );
    }

    event_dispatch();

    return 0;
}
