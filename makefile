SRC_DIR		:= src
INC_DIR		:= inc
OBJ_DIR		:= obj
BIN_DIR		:= bin

TARGET		:= $(BIN_DIR)/lvc

SOURCES		:= $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c $(SRC_DIR)/*/*/*.c)
OBJECTS		:= $(subst src/,obj/,$(SOURCES:.c=.o))
INC_DIRS	:= -I$(INC_DIR) $(addprefix -I,$(dir $(wildcard $(INC_DIR)/*/.)))

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

$(OBJECTS): obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(INC_DIRS)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)