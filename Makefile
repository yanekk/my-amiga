# to generate assembler sources: -Wa,-ahl=$@.s
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

forward-to-backward = $(subst /,\,$1)

subdirs := $(wildcard */)
VPATH = $(subdirs)
sources := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
asm_sources := $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/**/*.s)
objects := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(sources))
asm_objects := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(asm_sources))
all_objects = $(objects) $(asm_objects)

OUT = a.mingw
CC = m68k-amiga-elf-gcc

CCFLAGS = -g -MP -MMD -m68000 -Ofast -nostdlib -Wall -Wno-unused-function -Wno-volatile-register-var -fomit-frame-pointer -fno-tree-loop-distribution -flto -fwhole-program -fno-exceptions
ASFLAGS = -Wa,-g
LDFLAGS = -Wl,--emit-relocs,-Ttext=0,-Map=$(BIN_DIR)/$(OUT).map

all: $(BIN_DIR)/$(OUT).exe

$(BIN_DIR)/$(OUT).exe: $(BIN_DIR)/$(OUT).elf
	$(info Elf2Hunk $(OUT).exe)
	@elf2hunk $(BIN_DIR)/$(OUT).elf $(BIN_DIR)/$(OUT).exe

$(BIN_DIR)/$(OUT).elf: $(all_objects)
	$(info Linking a.mingw.elf)
	@if not exist "$(BIN_DIR)" mkdir $(BIN_DIR)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(all_objects) -o $@
	@m68k-amiga-elf-objdump --disassemble -S $@ >$(BIN_DIR)/$(OUT).s 

$(asm_objects): obj/%.o : %.s
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
