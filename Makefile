# to generate assembler sources: -Wa,-ahl=$@.s
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

forward-to-backward = $(subst /,\,$1)

subdirs := $(wildcard */)
VPATH = $(subdirs)
sources := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
objects := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(sources))

OUT = a.mingw
CC = m68k-amiga-elf-gcc

CCFLAGS = -g -MP -MMD -m68000 -Ofast -nostdlib -Wall -Wno-unused-function -Wno-volatile-register-var -fomit-frame-pointer -fno-tree-loop-distribution -flto -fwhole-program -fno-exceptions
ASFLAGS = -Wa,-g
LDFLAGS = -Wl,--emit-relocs,-Ttext=0,-Map=$(BIN_DIR)/$(OUT).map

all: $(BIN_DIR)/$(OUT).exe

$(BIN_DIR)/$(OUT).exe: $(BIN_DIR)/$(OUT).elf
	$(info Elf2Hunk $(OUT).exe)
	@elf2hunk $(BIN_DIR)/$(OUT).elf $(BIN_DIR)/$(OUT).exe

$(BIN_DIR)/$(OUT).elf: $(objects) $(OBJ_DIR)/support/gcc8_a_support.o
	$(info Linking a.mingw.elf)
	@mkdir $(BIN_DIR)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(objects) obj/support/gcc8_a_support.o -o $@
	@m68k-amiga-elf-objdump --disassemble -S $@ >$(BIN_DIR)/$(OUT).s 

$(OBJ_DIR)/support/gcc8_a_support.o: $(SRC_DIR)/support/gcc8_a_support.s
	$(info Assembling $<)
	@$(CC) $(CCFLAGS) $(ASFLAGS) -xassembler-with-cpp -c -o $@ $(CURDIR)/$<

clean:
	$(info Cleaning...)
	rm $(OBJ_DIR) $(BIN_DIR) -r

-include $(objects:.o=.d)

$(objects) : obj/%.o : %.c
	@if not exist "$(call forward-to-backward,$(dir $@))" mkdir $(call forward-to-backward,$(dir $@))
	$(info Compiling $<)
	@$(CC) $(CCFLAGS) -c -o $@ $(CURDIR)/$<
