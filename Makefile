# ======================================================================
# CONFIGURATION
# ======================================================================
# default test and architecture
TEST ?= fibonacci
RV64 ?= 0
M_EXT ?= 1

# core Directories
SDK_DIR      = sdk
BSP_DIR        = $(SDK_DIR)/bsp
LIBRV_DIR      = $(SDK_DIR)/librv
TESTS_DIR      = tests
TB_DIR         = $(TESTS_DIR)/$(TEST)

BUILD_BASE_DIR = build
EMU_BUILD_DIR  = $(BUILD_BASE_DIR)/emulator
TEST_BUILD_DIR = $(BUILD_BASE_DIR)/tests/$(TEST)

# ======================================================================
# EMULATOR (HOST) TOOLCHAIN via CMAKE
# ======================================================================
CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Release

ifeq ($(RV64), 1)
	CMAKE_FLAGS += -DRV64=ON
else
	CMAKE_FLAGS += -DRV64=OFF
endif

ifeq ($(M_EXT), 1)
	CMAKE_FLAGS += -DM_EXT=ON
else
	CMAKE_FLAGS += -DM_EXT=OFF
endif

# CMake output
ifeq ($(OS),Windows_NT)
    EMU_EXE = $(EMU_BUILD_DIR)/bin/rvem.exe
else
    EMU_EXE = $(EMU_BUILD_DIR)/bin/rvem
endif

# ======================================================================
# RISC-V GUEST SOFTWARE TOOLCHAIN
# ======================================================================
RV_PREFIX = riscv64-unknown-elf-
CC        = $(RV_PREFIX)gcc
OBJCOPY   = $(RV_PREFIX)objcopy
AR        = $(RV_PREFIX)ar

# adjust architecture
ifeq ($(RV64), 1)
	MARCH = rv64i
	MABI  = lp64
else
	MARCH = rv32i
	MABI  = ilp32
endif

ifeq ($(M_EXT), 1)
	MARCH := $(MARCH)m
endif

# guest configuration
LDSCRIPT = $(BSP_DIR)/link.ld
CRT_ASM  = $(BSP_DIR)/crt0.S
CFLAGS   = -march=$(MARCH) -mabi=$(MABI) -ffreestanding -nostdlib -O2
LIBGCC   = $(shell $(CC) -march=$(MARCH) -mabi=$(MABI) -print-libgcc-file-name 2>/dev/null)
ifeq ($(RV64), 1)
CLIBS    =
else
CLIBS    = $(LIBGCC)
endif

# artifacts
C_SRC     = $(wildcard $(TB_DIR)/*.c)
ELF_FILE  = $(TEST_BUILD_DIR)/$(TEST).elf
BIN_FILE  = $(TEST_BUILD_DIR)/$(TEST).bin

# librv artifacts (architecture specific)
LIBRV_SRC     = $(wildcard $(LIBRV_DIR)/*.c)
LIBRV_OBJ_DIR = $(BUILD_BASE_DIR)/librv_obj/$(MARCH)
LIBRV_OBJ     = $(patsubst $(LIBRV_DIR)/%.c, $(LIBRV_OBJ_DIR)/%.o, $(LIBRV_SRC))
LIBRV_ARCHIVE = $(BUILD_BASE_DIR)/librv_$(MARCH).a

# ======================================================================
# BUILD RULES
# ======================================================================
.PHONY: all emulate emulator clean test_dirs phase2_header

all: emulate

test_dirs:
	@mkdir -p $(EMU_BUILD_DIR)
	@mkdir -p $(TEST_BUILD_DIR)
	@mkdir -p $(LIBRV_OBJ_DIR)

phase2_header:
	@echo "========================================"
	@echo " Phase 2: Guest Software [$(MARCH)]"
	@echo "========================================"

# 1. build the emulator using CMake
emulator: test_dirs
	@echo "========================================"
	@echo " Phase 1: Host Emulator (RV64=$(RV64))"
	@echo "========================================"
	@cmake -S . -B $(EMU_BUILD_DIR) -G "Unix Makefiles" $(CMAKE_FLAGS) > /dev/null
	@cmake --build $(EMU_BUILD_DIR) --config Release -- --no-print-directory

# 2. compile librv
$(LIBRV_OBJ_DIR)/%.o: $(LIBRV_DIR)/%.c | test_dirs phase2_header
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -DPRINTF_INCLUDE_CONFIG_H -I$(LIBRV_DIR) -c $< -o $@

$(LIBRV_ARCHIVE): $(LIBRV_OBJ) | test_dirs
	@echo "  AR      $@"
	@$(AR) rcs $@ $^

# 3. compile RISC-V code to elf
$(ELF_FILE): $(C_SRC) $(CRT_ASM) $(LDSCRIPT) $(LIBRV_ARCHIVE) | test_dirs phase2_header
	@if [ -z "$(C_SRC)" ]; then \
		echo "ERROR: No .c files found in $(TB_DIR)."; \
		exit 1; \
	fi
	@echo "  CC/LD   $@"
	@$(CC) $(CFLAGS) -I$(LIBRV_DIR) -T $(LDSCRIPT) $(CRT_ASM) $(C_SRC) $(LIBRV_ARCHIVE) $(CLIBS) -o $(ELF_FILE)

# 4. extract raw binary ROM
$(BIN_FILE): $(ELF_FILE)
	@echo "  OBJCOPY $@"
	@$(OBJCOPY) -O binary -j .text -j .rodata -j .srodata -j .data $< $@

# 5. emulate
emulate: emulator $(BIN_FILE)
	@echo "========================================"
	@echo " Phase 3: Emulation -> $(TEST)"
	@echo "========================================"
	@$(EMU_EXE) $(BIN_FILE)

clean:
	@echo "Wiping build directory..."
	@-rm -rf $(BUILD_BASE_DIR)
