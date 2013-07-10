OPTIMIZATION = s

#-------------------------------------------------------------------------------

SRC_C  = comm.c config.c main.c injection.c idle.c math_ext.c pid.c sensors.c sync.c
SRC_C += system_stm32f4xx.c

SRC_ASM += startup_stm32f40xx.s

#-------------------------------------------------------------------------------

CROSS_PATH = H:/Development/openecu/tools/CodeSourcery/

CROSS_VERSION = 

CROSS = $(CROSS_PATH)$(CROSS_VERSION)/bin/arm-none-eabi-

INCLUDES += -I$(CROSS_PATH)/arm-none-eabi/include
INCLUDES += -I$(CROSS_PATH)/arm-none-eabi/include/lib
INCLUDES += -ICMSIS

VPATH += CMSIS

#-------------------------------------------------------------------------------

FLAGS_C  = $(INCLUDES) -I.
FLAGS_C += -O$(OPTIMIZATION)
FLAGS_C += -mlittle-endian -mthumb
FLAGS_C += -mcpu=cortex-m4
FLAGS_C += -std=c99

FLAGS_LD = -Xlinker -Map=target/target.map
FLAGS_LD += -Wl,--gc-sections
FLAGS_LD += -mcpu=cortex-m4
FLAGS_LD += -mthumb
FLAGS_LD += -static   
#FLAGS_LD += -stdlib
#FLAGS_LD += -nostartfiles

#LIB_LD = -lm

FLAGS_ASM  = -D__ASSEMBLY__
FLAGS_ASM += -g $(FLAGS_C)
FLAGS_ASM += -I. -x assembler-with-cpp

#-------------------------------------------------------------------------------

all: sperator target.elf

mcu_all: sperator target.elf mcu_prog
 
%.elf: $(SRC_ASM:%.S=target/%.o) $(SRC_C:%.c=target/%.o)
	@echo Linking: $@
	@$(CROSS)gcc $(FLAGS_LD) -TCMSIS/stm32f4xx_flash.ld -o target/$@ $^ $(LIB_LD)
	@echo '--------------------------------------------------------------------'
	@$(CROSS)size target/target.elf
	@$(CROSS)size target/target.elf > target/target.log
	@$(CROSS)objcopy -O binary target/target.elf target.bin
	@$(CROSS)objcopy -O ihex target/target.elf target.hex
	@$(CROSS)objdump -h -S -z target/target.elf > target/target.lss
	@$(CROSS)nm -n target/target.elf > target/target.sym
	@del /Q target

$(SRC_C:%.c=target/%.o): target/%.o: %.c
	@echo Compiling: $<
	@$(CROSS)gcc $(FLAGS_C) -c $< -o $@

$(SRC_ASM:%.s=target/%.o): target/%.o: %.s
	@echo Compiling asm: $<
	@$(CROSS)gcc $(FLAGS_ASM) -c $< -o $@

clean:
	@echo '--------------------------------------------------------------------'
	@echo > target/dummy.txt
#	@rm -f target/*.*

sperator:
	@echo '--------------------------------------------------------------------'

.PHONY : all clean mcu_prog mcu_reset
