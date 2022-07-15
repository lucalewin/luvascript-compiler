SRC_DIR		:= src
INC_DIR		:= inc
OBJ_DIR		:= obj
BIN_DIR		:= bin

SOURCES		:= $(shell find $(SRC_DIR) -name '*.c')
OBJECTS		:= $(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(SOURCES:.c=.o))

BIN_NAME	:= lvc

# paths and filenames for 'make debug'
DEBUG_OBJ_DIR	:= $(OBJ_DIR)/debug
DEBUG_BIN_DIR	:= $(BIN_DIR)/debug

DEBUG_OBJECTS	:= $(subst $(SRC_DIR)/,$(DEBUG_OBJ_DIR)/,$(SOURCES:.c=.o))
DEBUG_TARGET	:= $(DEBUG_BIN_DIR)/$(BIN_NAME)

# paths and filenames for 'make release'
RELEASE_OBJ_DIR	:= $(OBJ_DIR)/release
RELEASE_BIN_DIR	:= $(BIN_DIR)/release

RELEASE_OBJECTS	:= $(subst $(SRC_DIR)/,$(RELEASE_OBJ_DIR)/,$(SOURCES:.c=.o))
RELEASE_TARGET	:= $(RELEASE_BIN_DIR)/$(BIN_NAME)

# ------------------------------------

CC			:= gcc
CFLAGS		:= -c -std=c17 -Wall -Werror -I$(INC_DIR)#-Wextra
LDFLAGS		:= -g

# ------------------------------------

all: debug release

mkdirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DEBUG_OBJ_DIR)
	@mkdir -p $(DEBUG_BIN_DIR)
	@mkdir -p $(RELEASE_OBJ_DIR)
	@mkdir -p $(RELEASE_BIN_DIR)

debug: CFLAGS += -DDEBUG
debug: mkdirs debug_build

release: CFLAGS += -O3
release: mkdirs release_build

test: debug
	@python3 test.py

install: release
	cp $(RELEASE_TARGET) /usr/bin/$(BIN_NAME)

uninstall:
	rm /usr/bin/$(BIN_NAME)

clean: debug_clean release_clean

# --------- [Debug Targets] ----------

debug_build: $(DEBUG_TARGET)

debug_clean:
	rm -rf $(DEBUG_OBJECTS) $(DEBUG_TARGET)

debug_rebuild: debug_clean debug

$(DEBUG_OBJECTS): $(DEBUG_OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_DIRS) $< -o $@

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

# -------- [Release Targets] ---------

release_build: $(RELEASE_TARGET)

release_clean:
	rm -rf $(RELEASE_OBJECTS) $(RELEASE_TARGET)

release_rebuild: release_clean release

$(RELEASE_OBJECTS): $(RELEASE_OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_DIRS) $< -o $@

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
