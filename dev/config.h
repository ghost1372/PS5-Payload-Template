#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * Central place for every project-specific string and constant.
 * Change them here; nothing else in the code base hard-codes them.
 * Values wrapped in #ifndef can also be overridden from the Makefile (-D...).
 */

/* Name and version are injected by the Makefile (APP_NAME / PAYLOAD_VERSION). */
#ifndef APP_NAME
#define APP_NAME "MyPayload"
#endif
#ifndef PAYLOAD_VERSION
#define PAYLOAD_VERSION "dev"
#endif

/* Name shown in the PS5 running-process list (a thread name: 19 characters
 * at most, so keep APP_NAME short). */
#ifndef PAYLOAD_PROCESS_NAME
#define PAYLOAD_PROCESS_NAME "payload.elf"
#endif

/* Text shown in the web UI (sent to the page by GET /api/info, so they are
 * not hard-coded in web/index.html). Do not use " or \ in these strings. */
#define APP_TAGLINE      "A PS5 payload with a web UI"
#define APP_FOOTER_TEXT  "Developed by Your Name"

/* Web server */
#ifndef APP_PORT
#define APP_PORT 7788
#endif

/* Where the UI can be opened:
 *   1 = on the PS5 itself (its browser is launched automatically) and from any
 *       device on the network at http://<ps5-ip>:<APP_PORT>
 *   0 = only from another device (laptop, phone) at that address; the PS5
 *       browser is not launched. */
#ifndef APP_OPEN_IN_CONSOLE_BROWSER
#define APP_OPEN_IN_CONSOLE_BROWSER 1
#endif

/* Anti-CSRF header: a POST must carry "<name>: 1" or handlers can reject it
 * (see server_request_t.has_custom_header). Matched case-insensitively.
 * Browsers cannot add a custom header cross-origin without a preflight, which
 * the server never answers, so this blocks drive-by requests from other pages. */
#define APP_CSRF_HEADER  "X-MyPayload"

/* Console database (app.db). Only defined for a quick start: the template
 * itself never opens it. Needs SQLite: add sqlite/sqlite3.c and
 * sqlite/sqlite3.h (the Makefile then defines APP_HAS_SQLITE). */
#ifndef APP_DB_PATH
#define APP_DB_PATH "/system_data/priv/mms/app.db"
#endif

/* Suggested place to copy app.db to before modifying it. */
#ifndef APP_DB_BACKUP_PATH
#define APP_DB_BACKUP_PATH "/data/app.db.bak"
#endif

#endif
