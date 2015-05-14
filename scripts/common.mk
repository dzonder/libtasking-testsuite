CC      = $(LIBTASKING_TESTSUITE_TOOLCHAIN)gcc
OBJCOPY = $(LIBTASKING_TESTSUITE_TOOLCHAIN)objcopy

ROOTDIR = ../..

BINDIR  = $(ROOTDIR)/build
LIBDIR  = libs

COMMON_FLAGS = -g3

ifneq (,$(OPTIMIZATIONS))
COMMON_FLAGS += $(OPTIMIZATIONS)
else
COMMON_FLAGS += -O3
endif

ifneq (,$(ENABLE_TRACE))
CFLAGS  += -DENABLE_TRACE -DTASK_DEFAULT_STACK_SIZE=1024
endif

ifneq (,$(DISABLE_ASSERT))
CFLAGS  += -DDISABLE_ASSERT
endif

CFLAGS  += $(COMMON_FLAGS) -std=c99 -c -Wall -Werror \
           -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
           -DARM_MATH_CM4 -D__FPU_PRESENT=1 \
           -ffunction-sections -fdata-sections \
           $(addprefix -I$(ROOTDIR)/, $(INCDIR))

LDFLAGS += $(COMMON_FLAGS) -nostartfiles -Tmk22fn1movlk12.lds \
           -L$(ROOTDIR)/scripts \
           -Wl,--gc-sections

CFLAGSDIR := $(shell echo "$(CFLAGS)" | sha256sum | head -c 64)
OBJDIR := $(BINDIR)/$(CFLAGSDIR)

INCDIR  += $(shell cd $(ROOTDIR); find . -type d -name include)

SOURCES += tests/$(TARGET)/$(TARGET).c
SOURCES += $(shell cd $(ROOTDIR); find $(LIBDIR) -name '*.[cS]')

OBJECTS += $(patsubst %.c, %.o, $(filter %.c, $(SOURCES)))
OBJECTS += $(patsubst %.S, %.S.o, $(filter %.S, $(SOURCES)))
OBJECTS := $(addprefix $(OBJDIR)/, $(OBJECTS))

VPATH = $(ROOTDIR)

.SECONDARY :

.PHONY : all prog run test clean

all : $(BINDIR)/$(TARGET).bin

%.bin : %.elf
	$(OBJCOPY) $< $@ -O binary

%.elf : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BINDIR)/$(TARGET).d :
	$(CC) -MM $(CFLAGS) $(addprefix $(ROOTDIR)/, $(SOURCES)) > $@

ifneq ($(MAKECMDGOALS),clean)
  $(shell mkdir -p $(BINDIR) $(dir $(OBJECTS)))
  -include $(BINDIR)/$(TARGET).d
endif

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.S.o : %.S
	$(CC) $(CFLAGS) $< -o $@

prog : all
	$(ROOTDIR)/scripts/prog.sh $(BINDIR)/$(TARGET).bin

run : all
	@$(ROOTDIR)/scripts/gdb.sh $(TARGET)

test : all
	@$(ROOTDIR)/scripts/test.sh $(TARGET)

clean :
	@rm -rvf $(BINDIR)