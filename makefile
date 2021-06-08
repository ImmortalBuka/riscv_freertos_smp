###########################################################################################################
#paths
###########################################################################################################
GCC_PATH = /home/mmodin/buildenv/riscv64-unknown-elf-toolchain-10.2.0
GCC_INC_PATH = $(GCC_PATH)/riscv64-unknown-elf/include
GCC_INC_LIB_PATH = $(GCC_PATH)/lib/gcc/riscv64-unknown-elf/10.2.0/include
GCC_BIN_PATH = $(GCC_PATH)/bin/riscv64-unknown-elf-
#
GAS = $(GCC_BIN_PATH)as
GCC = $(GCC_BIN_PATH)gcc
GCP = $(GCC_BIN_PATH)g++
GLD = $(GCC_BIN_PATH)ld
GOC = $(GCC_BIN_PATH)objcopy
GOD = $(GCC_BIN_PATH)objdump
GSZ = $(GCC_BIN_PATH)size
GAR = $(GCC_BIN_PATH)ar
###########################################################################################################
#project
###########################################################################################################
PROJECT_NAME = project
SRC = main.cpp print.cpp std_system.cpp
SRC += heap_1.c list.c port.c queue.c tasks.c
VPATH = src src/freertos src/port
GOBJ = $(patsubst %.cpp, gcc/%.o, $(SRC))
GOBJ += $(patsubst %.c, gcc/%.o, $(SRC))
INC = -I'$(GCC_INC_PATH)' -I'inc' -I'$(GCC_INC_LIB_PATH)' -I'inc/freertos' -I'inc/port' 
###########################################################################################################
#common
###########################################################################################################
LLVM_GCC_C_CPP_COMMON = -O0 -g3 -Wextra -pedantic -ffreestanding -ffunction-sections -fno-common
LLVM_GCC_C_CPP_COMMON += -Wall -fdata-sections -save-temps=obj -fmessage-length=0 -Wunused -Wfloat-equal
LLVM_GCC_C_CPP_COMMON += -Wuninitialized -Wconversion -Wpointer-arith -Wpadded -Wshadow -Waggregate-return 
LLVM_GCC_C_CPP_COMMON += -Waddress-of-packed-member -fno-unroll-loops -Warray-bounds -Wmissing-declarations
LLVM_GCC_C_CPP_COMMON += -march=rv32imf -mabi=ilp32f $(INC)
#
LLVM_GCC_C_COMMON = -std=gnu2x -Werror=implicit-function-declaration -Wno-pointer-sign -Wmissing-prototypes
LLVM_GCC_C_COMMON += -Wno-pointer-sign -Wstrict-prototypes -Wbad-function-cast
#
LLVM_GCC_CPP_COMMON = -std=gnu++2a -Wctor-dtor-privacy -Weffc++ -Wsign-promo -fno-rtti -fno-use-cxa-atexit
LLVM_GCC_CPP_COMMON += -fno-threadsafe-statics
#
GCC_C_CPP_COMMON = -fsigned-char -fstack-usage -fopt-info-all-optall="gcc/opt.log"
GCC_C_CPP_COMMON += -Wlogical-op -fno-exceptions -Wmissing-attributes -Wattribute-alias -s -MMD -MP
GCC_C_CPP_COMMON += -MF"$(@:%.o=%.d)" -MT"$(@)" -v -c -o $@ $<
#
GCC_C_FLAGS = -Wno-discarded-qualifiers -nostdinc
#
GCC_CPP_FLAGS = -Wnoexcept -Wsign-promo -fno-rtti -nostdinc++
# 
LLVM_GCC_LINK_FLAGS = --gc-sections -T "${CURDIR}/ls.ld" -v --print-gc-sections --cref -o "$@"
#
GCC_LINK_FLAGS = --print-memory-usage -Map="gcc/$(PROJECT_NAME).map" --no-relax -m elf32lriscv
###########################################################################################################
#reciepes
###########################################################################################################
.PHONY:  build_gcc clean_gcc do_gcc

clean_gcc:
	rm -rf gcc
	mkdir gcc

do_gcc: gcc/$(PROJECT_NAME).lst gcc/$(PROJECT_NAME).bin gcc/$(PROJECT_NAME).hex

build_gcc: clean_gcc
	$(MAKE) do_gcc 2>&1 | tee -a gcc/make.log

gcc/%.o: %.cpp
	$(GCP) $(LLVM_GCC_C_CPP_COMMON) $(LLVM_GCC_CPP_COMMON) $(GCC_C_CPP_COMMON) $(GCC_CPP_FLAGS)

gcc/%.o: %.c
	$(GCC) $(LLVM_GCC_C_CPP_COMMON) $(LLVM_GCC_C_COMMON) $(GCC_C_CPP_COMMON) $(GCC_C_FLAGS)

gcc/$(PROJECT_NAME).elf: $(GOBJ)
	$(GLD) $(LLVM_GCC_LINK_FLAGS) $(GCC_LINK_FLAGS) $(wildcard gcc/*.o) 
	$(GSZ) --format=berkeley --radix=10 $@

gcc/$(PROJECT_NAME).bin: gcc/$(PROJECT_NAME).elf
	$(GOC) -O binary $< $@

gcc/$(PROJECT_NAME).lst: gcc/$(PROJECT_NAME).elf
	$(GOD) --source --all-headers --demangle --line-numbers --wide $< > $@

gcc/$(PROJECT_NAME).hex: gcc/$(PROJECT_NAME).elf
	$(GOC) -O ihex $< $@