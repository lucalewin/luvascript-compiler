SRC_DIR		:= src
INC_DIR		:= inc
OBJ_DIR		:= obj
BIN_DIR		:= bin

BIN_NAME	:= lvc

TARGET		:= $(BIN_DIR)/$(BIN_NAME)

SOURCES		:= $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c $(SRC_DIR)/*/*/*.c)
OBJECTS		:= $(subst src/,obj/,$(SOURCES:.c=.o))
INC_DIRS	:= -I$(INC_DIR)# $(addprefix -I,$(dir $(wildcard $(INC_DIR)/*/.)))

CC			:= gcc
CFLAGS		:= -g -c -std=c17 -Wall -Werror
LDFLAGS		:= -g

# ------------------------------------

all: build

build: dirs $(TARGET)

rebuild: clean build

test: build
	@python3 test.py

dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

install: build
	mv $(TARGET) /usr/bin/$(BIN_NAME)

uninstall:
	rm /usr/bin/$(BIN_NAME)

$(OBJECTS): obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_DIRS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)