#ifndef APP_WEBUI_H
#define APP_WEBUI_H

#include <stddef.h>

/*
 * Web UI module: the single-page web UI as a static string. It talks to the
 * server only through GET /api/info.
 */
const char *webui_page(void);
size_t webui_page_length(void);

#endif
