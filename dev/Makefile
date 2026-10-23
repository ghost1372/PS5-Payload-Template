PS5_HOST ?= ps5
PS5_PORT ?= 9021

ifdef PS5_PAYLOAD_SDK
    include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
    $(error PS5_PAYLOAD_SDK is undefined)
endif

APP_NAME ?= MyPayload
PAYLOAD_VERSION ?= v1.0.0
ELF := $(APP_NAME)-$(PAYLOAD_VERSION).elf

# SQLite is optional: drop sqlite3.c / sqlite3.h into sqlite/ to enable it
# (see sqlite/README.md). Without them the payload builds without SQLite.
SQLITE_SRC := $(wildcard sqlite/sqlite3.c)
SQLITE_HDR := $(wildcard sqlite/sqlite3.h)

SOURCES := main.c server.c webui.c $(SQLITE_SRC)
HEADERS := config.h server.h webui.h $(SQLITE_HDR)

CPPFLAGS := -DAPP_NAME=\"$(APP_NAME)\" \
            -DPAYLOAD_VERSION=\"$(PAYLOAD_VERSION)\"
ifneq ($(SQLITE_SRC),)
CPPFLAGS += -DAPP_HAS_SQLITE=1 \
            -DSQLITE_THREADSAFE=1 -DSQLITE_OMIT_LOAD_EXTENSION \
            -DSQLITE_OMIT_SHARED_CACHE -DSQLITE_TEMP_STORE=2
else
$(info note: sqlite/sqlite3.c not found, building without SQLite)
endif
CFLAGS := -Wall -Werror -Os
LDLIBS := -lkernel_sys -lSceUserService -lSceSystemService

.PHONY: all clean test debug

all: $(ELF)

$(ELF): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(SOURCES) $(LDLIBS)

clean:
	rm -f $(APP_NAME)-*.elf

test: $(ELF)
	$(PS5_DEPLOY) -h $(PS5_HOST) -p $(PS5_PORT) $^

debug: $(ELF)
	gdb-multiarch \
	-ex "set architecture i386:x86-64" \
	-ex "target extended-remote $(PS5_HOST):2159" \
	-ex "file $(ELF)" \
	-ex "remote put $(ELF) /data/$(ELF)" \
	-ex "set remote exec-file /data/$(ELF)" \
	-ex "start"
