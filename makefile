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
SRC += heap_1.c list.c port_smp.c queue.c tasks.c
VPATH = src src/freertos src/port
GOBJ = $(patsubst %.cpp, gcc/%.o, $(SRC))
GOBJ += $(patsubst %.c, gcc/%.o, $(SRC))
INC = -I'$(GCC_INC_PATH)' -I'inc' -I'$(GCC_INC_LIB_PATH)' -I'inc/freertos' -I'inc/port' 
###########################################################################################################
#common
###########################################################################################################
C_CPP_COMMON = -O0 -g3 -Wextra -pedantic -ffreestanding -ffunction-sections -fno-common -fanalyzer
C_CPP_COMMON += -Wall -fdata-sections -save-temps=obj -fmessage-length=0 -Wunused -Wfloat-equal
C_CPP_COMMON += -Wuninitialized -Wconversion -Wpointer-arith -Wpadded -Wshadow -Waggregate-return 
C_CPP_COMMON += -Waddress-of-packed-member -fno-unroll-loops -Warray-bounds=2 -Wmissing-declarations
C_CPP_COMMON += -march=rv32imf -mabi=ilp32f $(INC) -fsigned-char -fstack-usage -Wlogical-op
C_CPP_COMMON += -fopt-info-all-optall="gcc/opt.log" -fno-exceptions -Wmissing-attributes
C_CPP_COMMON += -Wattribute-alias -s -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -v -c -fomit-frame-pointer
#
C_COMMON = -std=gnu2x -Werror=implicit-function-declaration -Wno-pointer-sign -Wmissing-prototypes
C_COMMON += -Wno-pointer-sign -Wstrict-prototypes -Wbad-function-cast -Wno-discarded-qualifiers -nostdinc
#
CPP_COMMON = -std=gnu++2a -Wctor-dtor-privacy -Weffc++ -Wsign-promo -fno-rtti -fno-use-cxa-atexit
CPP_COMMON += -fno-threadsafe-statics -Wnoexcept -Wsign-promo -nostdinc++
# 
LINK_FLAGS = --gc-sections -T "${CURDIR}/ls.ld" -v --print-gc-sections --cref -o "$@"
LINK_FLAGS += --print-memory-usage -Map="gcc/$(PROJECT_NAME).map" --no-relax -m elf32lriscv
###########################################################################################################
#reciepes
###########################################################################################################
.PHONY: build_gcc clean_gcc do_gcc renode

clean_gcc:
	rm -rf gcc
	mkdir gcc

do_gcc: gcc/$(PROJECT_NAME).lst gcc/$(PROJECT_NAME).bin

build_gcc: clean_gcc
	$(MAKE) do_gcc 2>&1 | tee -a gcc/make.log

gcc/%.o: %.cpp
	$(GCP) $(CPP_COMMON) $(C_CPP_COMMON) -o $@ $<

gcc/%.o: %.c
	$(GCC) $(C_COMMON) $(C_CPP_COMMON) -o $@ $<

gcc/$(PROJECT_NAME).elf: $(GOBJ)
	$(GLD) $(LINK_FLAGS) $(wildcard gcc/*.o) 
	$(GSZ) --format=berkeley --radix=10 $@

gcc/$(PROJECT_NAME).bin: gcc/$(PROJECT_NAME).elf
	$(GOC) -O binary $< $@

gcc/$(PROJECT_NAME).lst: gcc/$(PROJECT_NAME).elf
	$(GOD) --source --all-headers --demangle --line-numbers --wide $< > $@

renode: gcc/$(PROJECT_NAME).elf
	renode renode/multi_core.resc
	