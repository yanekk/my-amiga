SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TEST_DIR = ./test

SRC_FILES := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

CC = m68k-amigaos-gcc
CFLAGS = -I.
CFLAGS += -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
#CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64
CFLAGS += -MMD
CFLAGS += -noixemul

-include $(OBJ_FILES:.o=.d)

EXE = main.exe
all: $(BIN_DIR)/$(EXE) $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(patsubst %/,%,$(sort $(dir $@)))
	$(CC) $(CFLAGS) -c -o $@ $< 

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm $(OBJ_DIR) $(BIN_DIR) -r
rebuild: clean all
.PHONY: all clean