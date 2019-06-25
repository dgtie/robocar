GCC = arm-none-eabi-gcc
GPP = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

#SRC = main.cpp
SRC = $(shell ls -r *.cpp)
OBJ = $(SRC:.cpp=.o)
SRCC = $(shell find ../system/ -name '*.c')
OBJC = $(SRCC:.c=.o)
SRCL = $(shell find ../lamos/ -name '*.cpp')
OBJL = $(SRCL:.cpp=.o)
TARGET = out

OBJS = ../system/_cxx.o $(OBJC) $(OBJL) $(OBJ)

CFLAGS = -mcpu=cortex-m3 -mthumb -Wall \
         -DSTM32F10X_MD -DHSE_VALUE=8000000 \
         -ffunction-sections -fdata-sections -ffreestanding
CPPFLAGS = -std=gnu++11 -I../system -fno-exceptions -fno-rtti \
           -fno-use-cxa-atexit -fno-threadsafe-statics
LDFLAGS = -T mem.ld -T libs.ld -T sections.ld -L../system -nostartfiles \
          -Xlinker --gc-sections \
          -Wl,-Map=$(TARGET).map --specs=nano.specs

all: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	$(SIZE) $<

$(TARGET).elf: $(OBJS)
	$(GPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(GPP) $(CFLAGS) $(CPPFLAGS) -O1 -I../lamos -o $@ -c $<
#-Wa,-adhlns=$@.lst

../system/_cxx.o: ../system/_cxx.cpp
	$(GPP) $(CFLAGS) $(CPPFLAGS) -Os -o $@ -c $<

../system/%.o: ../system/%.c
	$(GCC) $(CFLAGS) -Os -o $@ -c $<

../lamos/%.o: ../lamos/%.cpp
	$(GPP) $(CFLAGS) $(CPPFLAGS) -Os -o $@ -c $<

clean:
	rm -rf *.o $(TARGET).hex $(TARGET).elf

flash:
	stm32flash -b 115200 -w $(TARGET).hex -v -g 0x0 /dev/serial0

run:
	stm32flash -g 0x0 /dev/serial0
