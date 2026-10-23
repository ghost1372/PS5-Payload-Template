/*
 * Payload template - serves a small web UI from the PS5.
 *
 *   main.c    wiring: routes requests and starts everything
 *   server.c  minimal HTTP server (GET/POST), knows nothing about the app
 *   web/index.html  the web page, embedded into the payload at build time
 *   config.h  every project-specific string and constant
 *
 * Open the UI from any device on the same network at
 * http://<ps5-ip>:<APP_PORT>, and (if APP_OPEN_IN_CONSOLE_BROWSER is 1 in
 * config.h) on the PS5 browser, which is launched automatically.
 *
 * To add an API endpoint: add a branch in handle_request() and reply with
 * server_send(). To add a module (for example SQLite access), add its .c/.h
 * files and list them in the Makefile.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/syscall.h>

#include "config.h"
#include "server.h"

/*
 * Embed web/index.html into the payload at build time (no external tools
 * needed). Defines: const uint8_t web_index_html[]; const size_t web_index_html_size;
 * The path is relative to the directory `make` runs in.
 */
#define INCASSET(name, file)                    \
    __asm__(".section .rodata\n"                \
            ".global " #name "\n"               \
            ".global " #name "_end\n"           \
            ".global " #name "_size\n"          \
            ".align 16\n"                       \
            #name ":\n"                         \
            ".incbin \"" file "\"\n"            \
            #name "_end:\n"                     \
            #name "_size:\n"                    \
            ".quad " #name "_end - " #name "\n" \
            ".previous\n");                     \
    extern const uint8_t name[];                \
    extern const size_t name##_size;

INCASSET(web_index_html, "web/index.html");

/* The process-list name is a thread name: 19 characters at most. */
_Static_assert(sizeof(PAYLOAD_PROCESS_NAME) <= 20,
               "PAYLOAD_PROCESS_NAME must be at most 19 characters");

int sceUserServiceInitialize(void *arg);
int sceSystemServiceLaunchWebBrowser(const char *uri, void *arg);

static void handle_request(int fd, const server_request_t *req, void *user) {
    (void)user;

    if(strcmp(req->method, "GET") != 0) {
        server_send(fd, 405, "text/plain", "Method not allowed", 18, 0);
        return;
    }

    if(strcmp(req->path, "/") == 0 || strcmp(req->path, "/index.html") == 0) {
        server_send(fd, 200, "text/html; charset=utf-8",
                    (const char *)web_index_html, web_index_html_size, 0);
    } else if(strcmp(req->path, "/api/info") == 0) {
        char ip[64];
        char body[512];

        /* name/tagline/footer come from config.h; avoid " and \ in them. */
        server_local_ip(ip, sizeof ip);
        snprintf(body, sizeof body,
                 "{\"ip\":\"%s\",\"port\":%d,\"version\":\"%s\","
                 "\"name\":\"%s\",\"tagline\":\"%s\",\"footer\":\"%s\"}",
                 ip, APP_PORT, PAYLOAD_VERSION, APP_NAME, APP_TAGLINE,
                 APP_FOOTER_TEXT);
        server_send(fd, 200, "application/json; charset=utf-8", body,
                    strlen(body), 0);
    } else {
        server_send(fd, 404, "text/plain", "Not found", 9, 0);
    }
}

/* Opens the UI on the console itself, unless disabled in config.h. */
static void launch_ps5_browser(void) {
#if APP_OPEN_IN_CONSOLE_BROWSER
    char url[64];

    snprintf(url, sizeof url, "http://127.0.0.1:%d", APP_PORT);
    (void)sceSystemServiceLaunchWebBrowser(url, NULL);
#endif
}

int main(void) {
    int listen_fd;

    /* Name shown in the running-process list (see config.h). */
    syscall(SYS_thr_set_name, -1, PAYLOAD_PROCESS_NAME);

#if APP_OPEN_IN_CONSOLE_BROWSER
    (void)sceUserServiceInitialize(NULL);
#endif

    listen_fd = server_start(APP_PORT);
    if(listen_fd < 0) {
        /* Port busy: an earlier instance is still serving, just open its UI. */
        launch_ps5_browser();
        return 0;
    }

    launch_ps5_browser();
    server_run(listen_fd, handle_request, NULL);
    return 0;
}
