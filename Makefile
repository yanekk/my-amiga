SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TEST_DIR = ./test

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

CC = m68k-amigaos-gcc
CFLAGS = -I.
CFLAGS += -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64
CFLAGS += -MMD
CFLAGS += -noixemul

-include $(OBJ_FILES:.o=.d)

EXE = main.exe
all: $(BIN_DIR)/$(EXE) $(OBJ_FILES)

.PHONY: test
test: tests.exe

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d $(BIN_DIR)/*.*

run: $(EXE)
	.\$(EXE)
.PHONY: all clean run 

TEST_OBJ_FILES := $(filter-out ./obj/main.o,$(OBJ_FILES))
tests.exe: $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) test/vendor/unity.c test/*.c $^ -o $(BIN_DIR)/$@ $(LIBS)