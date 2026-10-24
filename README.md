# PS5 Payload Template

A starting point for writing PS5 payloads that come with a **web UI**. The boring parts are already done, so you can go straight to your own idea:

- **HTTP server** is ready (`server.c`): small, single-threaded, GET and POST, JSON or HTML or file responses.
- **Web UI** is ready (`webui.c`): a clean single page served from the payload. It shows the console address (`ip:port`) and a footer text.
- **Configuration** is in one file (`config.h`): name, texts, port, database paths, and more.
- **SQLite** is wired into the Makefile, and the console database paths are already defined.
- **Console browser** is launched automatically, or turned off if you only want to use a laptop/PC or any other devices.

Out of the box the payload does nothing except show its own page. Everything else is yours to add.

![PS5-Payload-Template](https://raw.githubusercontent.com/ghost1372/App-Resources/refs/heads/main/PS5-Payloads/PS5-Payload-Template/1.png)

## Project layout

| File | Purpose |
|------|---------|
| `config.h` | Every project-specific string and constant. Start here. |
| `main.c` | Startup and request routing (`handle_request()`). |
| `server.c` / `server.h` | Minimal HTTP server. Knows nothing about your app. |
| `index.html` | The web page. |
| `sqlite/` | Put `sqlite3.c` and `sqlite3.h` here (optional, see below). |
| `Makefile` | Build, deploy and debug targets. |

## Build

1. **Set up the PS5 Payload SDK first.** Follow the instructions in the SDK repository and install it:
   <https://github.com/ps5-payload-dev/sdk>

2. **Point `PS5_PAYLOAD_SDK` to the SDK**, for example:

   ```sh
   export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
   ```

3. **(Optional) Add SQLite.** Download the amalgamation from <https://sqlite.org/download.html> and copy `sqlite3.c` and `sqlite3.h` into the `sqlite/` folder. The Makefile detects them automatically. Without them the payload still builds, just without SQLite.

4. **Build:**

   ```sh
   make
   ```

   The result is `<APP_NAME>-<PAYLOAD_VERSION>.elf`, for example `MyPayload-v1.0.0.elf`.

## Run

Send the `.elf` to your console with your usual payload sender:

Then open the UI:

- **On the console:** the PS5 browser opens automatically (when `APP_OPEN_IN_CONSOLE_BROWSER` is `1`).
- **From a laptop or phone on the same network:** `http://<ps5-ip>:7788` (or your `APP_PORT`).

## Adding an API endpoint

All requests go through `handle_request()` in `main.c`. Add a branch and reply with `server_send()`:

```c
} else if(strcmp(req->path, "/api/hello") == 0) {
    const char *body = "{\"hello\":\"world\"}";
    server_send(fd, 200, "application/json; charset=utf-8", body,
                strlen(body), 0);
}
```

Then call it from the page in `webui.c`, for example with `fetch('/api/hello')`.

**Endpoints that change something** should be POST requests and should check `req->has_custom_header`. The page must send the header named in `APP_CSRF_HEADER` with value `1`:

```js
fetch('/api/do-something', { method: 'POST', headers: { 'X-MyPayload': '1' } })
```

Browsers cannot add a custom header to a request from another website without a preflight, which this server never answers. This blocks other web pages on your network from triggering your endpoints.

## Adding your own modules

Create `mymodule.c` and `mymodule.h`, then add them to `SOURCES` and `HEADERS` in the Makefile. To use SQLite, `#include "sqlite/sqlite3.h"` and open `APP_DB_PATH`. Back up the database to `APP_DB_BACKUP_PATH` before writing to it.

## Troubleshooting

- **`PS5_PAYLOAD_SDK is undefined`**: export the variable as shown above.
- **UI does not open from my laptop**: check that the console and the laptop are on the same network, and that nothing else uses the port. If the port is taken, the payload just opens the UI of the instance that is already running.
- **SQLite message during `make`**: "sqlite/sqlite3.c not found" is only a note. Add the files if you need SQLite.
