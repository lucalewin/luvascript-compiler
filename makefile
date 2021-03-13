CC=gcc
CFLAGS= -c -std=c11 -g
LDFLAGS= -g

SRC=$(wildcard src/*.c)
#INC=$(wildcard src/include/*.h)
OBJ=$(subst src/,obj/,$(SRC:.c=.o))
TARGET=bin/lvc
TESTS=$(wildcard tests/*lvs)

# ------------------

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c #$(INC)
	$(CC) $(CFLAGS) $< -o $@

# ------------------

all: build

build: $(SRC) $(OBJ) $(TARGET)

rebuild: clean build

run: build
	$(TARGET)

test: $(TESTS) build
	$(TARGET) $(TESTS)

install:

clean:
	rm -rf $(OBJ) $(TARGET)