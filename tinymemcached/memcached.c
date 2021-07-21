#include "memcached.h"

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <ctype.h>
#include <stdarg.h>
#include <pwd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sysexits.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * forward declarations
 */
//static void drive_machine(conn *c);
//static int new_socket(struct addrinfo *ai);
static ssize_t tcp_read(conn *arg, void *buf, size_t count);
static ssize_t tcp_sendmsg(conn *arg, struct msghdr *msg, int flags);
static ssize_t tcp_write(conn *arg, void *buf, size_t count);

static int stop_main_loop = NOT_STOP;
static conn *listen_conn = NULL;
static int max_fds;
struct event_base *main_base;

conn *conn_new(const int sfd, enum conn_states init_state,
                const int event_flags,
                const int read_buffer_size, enum network_transport transport,
                struct event_base *base, void *ssl) {
    return NULL;
}

static int server_socket(const char *interface,
                         int port,
                         enum network_transport transport,
                         FILE *portnumber_file, bool ssl_enabled) {
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    int flags;

    if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
        fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("setting O_NONBLOCK");
        close(sfd);
        return -1;
    }

    struct sockaddr_in local_addr = {0};
	local_addr.sin_family = AF_INET;//使用IPv4
	local_addr.sin_port = htons(port);//port是端口号，网络传输使用大端字节序，使用htons()进行转换。端口是int类型，范围0~65535
	local_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sfd,(struct sockaddr *)&local_addr,sizeof(local_addr));

    conn *listen_conn_add;
    listen_conn_add = conn_new(sfd, conn_listening, EV_READ | EV_PERSIST, 1,
                             transport, main_base, NULL);
    listen_conn_add->next = listen_conn;
    listen_conn = listen_conn_add;

    return 1;
}

static int server_sockets(int port, enum network_transport transport, FILE *portnumber_file) {
    return server_socket(NULL, port, transport, NULL, false);
}

/* Default methods to read from/ write to a socket */
ssize_t tcp_read(conn *c, void *buf, size_t count) {
    assert (c != NULL);
    return read(c->sfd, buf, count);
}

ssize_t tcp_sendmsg(conn *c, struct msghdr *msg, int flags) {
    assert (c != NULL);
    return sendmsg(c->sfd, msg, flags);
}

ssize_t tcp_write(conn *c, void *buf, size_t count) {
    assert (c != NULL);
    return write(c->sfd, buf, count);
}



int main() {
    struct event_config *ev_config;
    ev_config = event_config_new();
    event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK);
    main_base = event_base_new_with_config(ev_config);
    event_config_free(ev_config);

    event_base_free(main_base);

    return 0;
}
