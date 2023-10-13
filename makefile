#Nombre del proyecto
TARGET = temp
#Archivos a compilar
SRCS  = main.c app_ints.c app_msps.c startup_stm32g0b1xx.s system_stm32g0xx.c 
SRCS += stm32g0xx_hal.c stm32g0xx_hal_cortex.c stm32g0xx_hal_rcc.c stm32g0xx_hal_flash.c
SRCS += stm32g0xx_hal_gpio.c stm32g0xx_hal_fdcan.c stm32g0xx_hal_tim.c stm32g0xx_hal_tim_ex.c stm32g0xx_hal_rcc_ex.c stm32g0xx_hal_rtc.c stm32g0xx_hal_rtc_ex.c
SRCS += stm32g0xx_hal_pwr.c stm32g0xx_hal_pwr_ex.c app_serial.c app_clock.c stm32g0xx_hal_spi.c hel_lcd.c app_can.c
#archivo linker a usar
LINKER = linker.ld
#Simbolos gloobales del programa (#defines globales)
SYMBOLS = -DSTM32G0B1xx -DUSE_HAL_DRIVER
#directorios con archivos a compilar (.c y .s)
SRC_PATHS  = app
SRC_PATHS += cmsisg0/startups
SRC_PATHS += halg0/Src
#direcotrios con archivos .h
INC_PATHS  = app
INC_PATHS += cmsisg0/core
INC_PATHS += cmsisg0/registers
INC_PATHS += halg0/Inc

TOOLCHAIN = arm-none-eabi
CPU = -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft

#opciones de compilacion
CFLAGS  = $(CPU)
CFLAGS += -O0                        # Compile with Size Optimizations (O0, O1, O2, O3, Os)
CFLAGS += -g3                        # Debugging information level (g1, g2, g3)
CFLAGS += -ffunction-sections        # Create a separate function section
CFLAGS += -fdata-sections            # Create a separate data section
CFLAGS += -fno-builtin               # Don't recognize built-in functions that do not begin with ‘__builtin_’ as prefix
CFLAGS += -std=c99                   # Comply with C11
CFLAGS += -Wall                      # Be anal Enable All Warnings
CFLAGS += -pedantic                  # Be extra anal More ANSI Checks
CFLAGS += -Wstrict-prototypes        # Warn if a function is declared or defined without specifying the argument types
CFLAGS += -fsigned-char              # char is treated as signed
CFLAGS += -fdiagnostics-color=always # color the output
CFLAGS += -fomit-frame-pointer       # Don't keep the frame pointer in a register for functions that don't need one
CFLAGS += -fverbose-asm              # Put extra commentary information in the generated assembly code
CFLAGS += -MMD -MP

#opciones de ensamblador
AFLAGS = $(CPU)

#opciones de linker
LFLAGS  = $(CPU) 
LFLAGS += -Wl,--gc-sections
LFLAGS += --specs=rdimon.specs 			# link with semihosting 
LFLAGS += --specs=nano.specs 			# nano version of stdlib
LFLAGS += -Wl,-Map=Build/$(TARGET).map	# Generate map file 

#Linter ccpcheck flags
LNFLAGS  = --inline-suppr       # comments to suppress lint warnings
LNFLAGS += --quiet              # spit only useful information
LNFLAGS += --std=c99            # check against C11
LNFLAGS += --template=gcc       # display warning gcc style
LNFLAGS += --force              # evaluate all the #if sentences
LNFLAGS += --platform=unix32    # lint againt a unix32 platform, but we are using arm32
LNFLAGS += --cppcheck-build-dir=Build/checks

#substituccion de prefijos y postfijos 
OBJS = $(SRCS:%.c=Build/obj/%.o)
OBJS := $(OBJS:%.s=Build/obj/%.o)

DEPS = $(OBJS:%.o=%.d)
VPATH = $(SRC_PATHS)
INCLS = $(addprefix -I ,$(INC_PATHS))

#Instrucciones de compilacion
all : build $(TARGET)

$(TARGET) : $(addprefix Build/, $(TARGET).elf)
	$(TOOLCHAIN)-objcopy -Oihex $< Build/$(TARGET).hex
	$(TOOLCHAIN)-objdump -S $< > Build/$(TARGET).lst
	$(TOOLCHAIN)-size --format=berkeley $<

Build/$(TARGET).elf : $(OBJS)
	$(TOOLCHAIN)-gcc $(LFLAGS) -T $(LINKER) -o $@ $^

Build/obj/%.o : %.c
	$(TOOLCHAIN)-gcc $(CFLAGS) $(INCLS) $(SYMBOLS) -o $@ -c $<

Build/obj/%.o : %.s
	$(TOOLCHAIN)-as $(AFLAGS) -o $@ -c $<

build :
	mkdir -p Build/obj

-include $(DEPS)

#borrar archivos generados
clean :
	rm -rf Build

#---flash the image into the mcu-------------------------------------------------------------------
flash :
	openocd -f board/st_nucleo_g0.cfg -c "program Build/$(TARGET).hex verify reset" -c shutdown

#---open a debug server conection------------------------------------------------------------------
open :
	openocd -f board/st_nucleo_g0.cfg
#	JLinkGDBServer -if SWD -device stm32g0b1re -nogui

#---launch a debug session, NOTE: is mandatory to previously open a debug server session-----------
debug :
	arm-none-eabi-gdb Build/$(TARGET).elf -iex "set auto-load safe-path /"

#---Genrete project documentation with doxygen-----------------------------------------------------
docs :
	doxygen .doxyfile
	firefox Build/doxygen/html/index.html

#---Run Static analysis
lint :
	mkdir -p Build/checks
	cppcheck --addon=misra.json --suppressions-list=.msupress $(LNFLAGS) app
