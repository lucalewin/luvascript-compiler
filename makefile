PROJ_DIR	:= $(CURDIR)

SRC_DIR		:= src
INC_DIRS	:= -I./inc $(addprefix -I./,$(dir $(wildcard inc/*/.))) # all include folders with depth=1
OBJ_DIR		:= obj
BIN_DIR		:= bin

TARGET		:= $(BIN_DIR)/lvc

SOURCES		:= $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c)
OBJECTS		:= $(subst src/,obj/,$(SOURCES:.c=.o))

CC			:= gcc
CFLAGS		:= -c -std=c17 -g
LDFLAGS		:= -g


# ------------------------------------

all: dirs $(TARGET)

$(OBJECTS): obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(INC_DIRS)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@


dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)


clean:
	@rm -rf $(OBJECTS) $(TARGET)


# @echo $(SOURCES)
# @echo ---- OBJECTS ----
# @echo $(OBJECTS)
# CC=gcc
# CFLAGS= -c -std=c17 -g
# LDFLAGS= -g

# SRC=$(wildcard src/*.c)
# OBJ=$(subst src/,obj/,$(SRC:.c=.o))
# TARGET=bin/lvc
# TESTS=$(wildcard tests/*lvs)

# # ------------------

# $(TARGET): $(OBJ)
# 	$(CC) $(LDFLAGS) $^ -o $@

# obj/%.o: src/%.c
# 	$(CC) $(CFLAGS) $< -o $@

# # ------------------

# all: build

# build: $(SRC) $(OBJ) $(TARGET)

# rebuild: clean build

# run: build
# 	$(TARGET)

# test: $(TESTS) build
# 	$(TARGET) $(TESTS)

# install:

# clean:
# 	rm -rf $(OBJ) $(TARGET)