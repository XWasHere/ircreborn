#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <ui/window.h>
#include <ui/widgets/button.h>
#include <ui/widgets/scrollpane.h>
#include <common/util.h>
#include <common/args.h>
#include <config_parser/config.h>
#include <networking/networking.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#ifdef WIN32
#include <windows.h>
#define ERRORNO GetLastError
#else
#include <errno.h>
#define ERRORNO errno
#endif

#ifdef WIN32
WSADATA* wsadata;
#endif 

struct server {
    char* host;
    int   port;
    char* name;
    
    widget_t* button;
};

struct server** servers;
int             server_count;

// server connection
int sc;
int sc_connected;

int nextpos = 0;

void exit_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    printf(FMT_INFO("user requested exit. goodbye\n"));
    exit(0);
}

void server_list_collapse_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    printf(FMT_INFO("server list collapsed\n"));
}

void server_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    for (int i = 0; i < server_count; i++) {
        if (servers[i]->button == widget) {
            printf(FMT_INFO("connecting to server %s\n"), servers[i]->name);
            
            if ((sc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                sc = 0;
                printf(FMT_FATL("socket(): %i\n"), ERRORNO());
                return;
            }

            struct sockaddr_in* addr = malloc(sizeof(struct sockaddr_in));

            addr->sin_family = AF_INET;
            addr->sin_port   = htons(servers[i]->port);

            inet_pton(AF_INET, servers[i]->host, &addr->sin_addr);

            if (connect(sc, addr, sizeof(struct sockaddr)) == -1) {
                printf(FMT_FATL("connect(): %s"), format_error(WSAGetLastError()));
            }

            hello_t* hi = malloc(sizeof(hello_t));
            memset(hi, 0, sizeof(hello_t));

            hi->has_ident = 1; // we have an identity :)
            hi->ident     = "xutils ircreborn client";

            send_hello(sc, hi);

            return;
        }
    }
}

struct server* server_list_add_server(widget_t* serverlist, char* name, char* host, int port) {
    struct server* s = malloc(sizeof(struct server));
    s->button = button_init();
    s->name = name;
    s->host = host;
    s->port = port;
    button_t* btn = s->button->extra_data;

    s->button->height = 20;
    s->button->width  = serverlist->width - 20;
    s->button->clicked= &server_button_clicked;

    btn->type = BUTTON_TEXT;
    btn->text = name;

    scroll_pane_item_t* item = scroll_pane_add_item(serverlist, s->button);
    item->x = 0;
    item->y = nextpos;
    nextpos += 20;

    server_count++;
    servers = realloc(servers, sizeof(void*) * server_count);
    servers[server_count - 1] = s;

    return s;
}

void client_main() {
#ifdef WIN32
    wsadata = malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        printf(FMT_FATL("failed to start winsock, aborting\n"));
        printf(FMT_FATL("%s"), format_error(WSAGetLastError()));
        exit(1);
    }
#endif

    printf(FMT_INFO("reading config from %s\n"), args_config_path);

    int configfd = open(args_config_path, O_RDONLY | O_CREAT);
    chmod(args_config_path, S_IWUSR | S_IRUSR);
    config_t* config = parse_config(configfd);
    close(configfd);

    servers = malloc(1);

    window_t* main_window = window_init();
    
    widget_t* exitbtnw = button_init();
    button_t* exitbtne = exitbtnw->extra_data;
    widget_t* serverlistw = scroll_pane_init();
    scroll_pane_t* serverliste = serverlistw->extra_data;
    widget_t* serverlistcollapsebtnw = button_init();
    button_t* serverlistcollapsebtne = serverlistcollapsebtnw->extra_data;
    
    exitbtnw->x = 0;
    exitbtnw->y = 0;
    exitbtnw->width = 40;
    exitbtnw->height = 20;
    exitbtnw->clicked = &exit_button_clicked;
    exitbtne->type = BUTTON_TEXT;
    exitbtne->text = "exit";
    
    serverlistw->x = 0;
    serverlistw->y = 20;
    serverlistw->width = 200;
    serverlistw->height = 400;
    
    serverlistcollapsebtnw->x = 200;
    serverlistcollapsebtnw->y = 20;
    serverlistcollapsebtnw->width = 20;
    serverlistcollapsebtnw->height = 20;
    serverlistcollapsebtnw->clicked = &server_list_collapse_button_clicked;
    serverlistcollapsebtne->type = BUTTON_TEXT;
    serverlistcollapsebtne->text = "<";
    
    
    window_add_widget(main_window, exitbtnw);
    window_add_widget(main_window, serverlistw);
    window_add_widget(main_window, serverlistcollapsebtnw);

    for (int i = 0; i < config->server_count; i++) {
        server_list_add_server(serverlistw, config->servers[i]->name, config->servers[i]->host, config->servers[i]->port);
    }

    window_display(main_window);
}
