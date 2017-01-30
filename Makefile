# Project (adjust to match your needs)
PROJECT=hello
ELF = $(PROJECT).elf

# Library paths (adjust to match your needs)
STM32F4CUBE=../stm32f4cube
CMSIS=$(STM32F4CUBE)/Drivers/CMSIS
HAL=$(STM32F4CUBE)/Drivers/STM32F4xx_HAL_Driver

# Toolchain paths (adjust to match your needs)
CC=armv7m-hardfloat-eabi-gcc
CXX=armv7m-hardfloat-eabi-g++
LD=armv7m-hardfloat-eabi-ld
OBJCOPY=armv7m-hardfloat-eabi-objcopy
SIZE=armv7m-hardfloat-eabi-size
GDB=armv7m-hardfloat-eabi-gdb
OPENOCD=openocd

# Compiler and linker options
CFLAGS = -mcpu=cortex-m4 -g -Os -Wall -pipe
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP -fsingle-precision-constant
CFLAGS += -D STM32F407xx
CXXFLAGS = $(CFLAGS)
LDFLAGS=-T STM32F407VG_FLASH.ld -specs=nosys.specs

# Includes including library includes
INCLUDES=\
-I./inc \
-I$(HAL)/Inc \
-I$(CMSIS)/Device/ST/STM32F4xx/Include \
-I$(CMSIS)/Include

# Application objects
APP_OBJECTS=\
src/stm32f4xx_it.o \
src/system_stm32f4xx.o \
src/startup_stm32f407xx.o \
src/main.o

# Currenly used HAL module objects
HAL_OBJECTS=\
$(HAL)/Src/stm32f4xx_hal.o \
$(HAL)/Src/stm32f4xx_hal_gpio.o \
$(HAL)/Src/stm32f4xx_hal_tim.o \
$(HAL)/Src/stm32f4xx_hal_tim_ex.o \
$(HAL)/Src/stm32f4xx_hal_rcc.o \
$(HAL)/Src/stm32f4xx_hal_rcc_ex.o \
$(HAL)/Src/stm32f4xx_hal_dma.o \
$(HAL)/Src/stm32f4xx_hal_dma_ex.o \
$(HAL)/Src/stm32f4xx_hal_cortex.o \
\
$(HAL)/Src/stm32f4xx_hal_usart.o \
$(HAL)/Src/stm32f4xx_hal_uart.o \

# Available HAL module objects
HAL_OBJECTS_EXTRA=\
$(HAL)/Src/stm32f4xx_hal_wwdg.o \
$(HAL)/Src/stm32f4xx_ll_fmc.o \
$(HAL)/Src/stm32f4xx_ll_fsmc.o \
$(HAL)/Src/stm32f4xx_ll_sdmmc.o \
$(HAL)/Src/stm32f4xx_ll_usb.o \
$(HAL)/Src/stm32f4xx_hal_hash.o \
$(HAL)/Src/stm32f4xx_hal_hash_ex.o \
$(HAL)/Src/stm32f4xx_hal_hcd.o \
$(HAL)/Src/stm32f4xx_hal_i2c.o \
$(HAL)/Src/stm32f4xx_hal_i2c_ex.o \
$(HAL)/Src/stm32f4xx_hal_i2s.o \
$(HAL)/Src/stm32f4xx_hal_i2s_ex.o \
$(HAL)/Src/stm32f4xx_hal_irda.o \
$(HAL)/Src/stm32f4xx_hal_iwdg.o \
$(HAL)/Src/stm32f4xx_hal_lptim.o \
$(HAL)/Src/stm32f4xx_hal_ltdc.o \
$(HAL)/Src/stm32f4xx_hal_ltdc_ex.o \
$(HAL)/Src/stm32f4xx_hal_nand.o \
$(HAL)/Src/stm32f4xx_hal_nor.o \
$(HAL)/Src/stm32f4xx_hal_pccard.o \
$(HAL)/Src/stm32f4xx_hal_pcd.o \
$(HAL)/Src/stm32f4xx_hal_pcd_ex.o \
$(HAL)/Src/stm32f4xx_hal_pwr.o \
$(HAL)/Src/stm32f4xx_hal_pwr_ex.o \
$(HAL)/Src/stm32f4xx_hal_qspi.o \
$(HAL)/Src/stm32f4xx_hal_rng.o \
$(HAL)/Src/stm32f4xx_hal_rtc.o \
$(HAL)/Src/stm32f4xx_hal_rtc_ex.o \
$(HAL)/Src/stm32f4xx_hal_sai.o \
$(HAL)/Src/stm32f4xx_hal_sai_ex.o \
$(HAL)/Src/stm32f4xx_hal_sdram.o \
$(HAL)/Src/stm32f4xx_hal_smartcard.o \
$(HAL)/Src/stm32f4xx_hal_spdifrx.o \
$(HAL)/Src/stm32f4xx_hal_spi.o \
$(HAL)/Src/stm32f4xx_hal_sram.o \
$(HAL)/Src/stm32f4xx_hal_adc.o \
$(HAL)/Src/stm32f4xx_hal_adc_ex.o \
$(HAL)/Src/stm32f4xx_hal_can.o \
$(HAL)/Src/stm32f4xx_hal_cec.o \
$(HAL)/Src/stm32f4xx_hal_crc.o \
$(HAL)/Src/stm32f4xx_hal_cryp.o \
$(HAL)/Src/stm32f4xx_hal_cryp_ex.o \
$(HAL)/Src/stm32f4xx_hal_dac.o \
$(HAL)/Src/stm32f4xx_hal_dac_ex.o \
$(HAL)/Src/stm32f4xx_hal_dcmi.o \
$(HAL)/Src/stm32f4xx_hal_dcmi_ex.o \
$(HAL)/Src/stm32f4xx_hal_dfsdm.o \
$(HAL)/Src/stm32f4xx_hal_dma2d.o \
$(HAL)/Src/stm32f4xx_hal_dsi.o \
$(HAL)/Src/stm32f4xx_hal_eth.o \
$(HAL)/Src/stm32f4xx_hal_flash.o \
$(HAL)/Src/stm32f4xx_hal_flash_ex.o \
$(HAL)/Src/stm32f4xx_hal_flash_ramfunc.o \
$(HAL)/Src/stm32f4xx_hal_fmpi2c.o \
$(HAL)/Src/stm32f4xx_hal_fmpi2c_ex.o


all: $(ELF)

clean:
	rm -f $(ELF)
	rm -f **/*.o
	rm -f **/*.d
	rm -f $(HAL_OBJECTS)

# Link final efl
$(ELF): $(APP_OBJECTS) $(HAL_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Compile C source into object
%.o : %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

# Compile assembler source into object
%.o : %.s
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

# Flash final elf into device
flash: $(ELF)
	${OPENOCD} -f board/stm32f4discovery.cfg -c "program $< verify reset exit"

# Debug
debug: $(ELF)
	$(GDB) $(ELF) -ex "target remote | openocd -f board/stm32f4discovery.cfg --pipe" -ex load

.PHONY: all flash clean debug
