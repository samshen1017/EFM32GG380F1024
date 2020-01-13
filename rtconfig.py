import os

# toolchains options
ARCH        = 'arm'
CPU         = 'cortex-m3'
CROSS_TOOL  = 'gcc'

if os.getenv('RTT_CC'):
	CROSS_TOOL = os.getenv('RTT_CC')
if os.getenv('RTT_ROOT'):
	RTT_ROOT = os.getenv('RTT_ROOT')
	
# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
if CROSS_TOOL == 'gcc':
	PLATFORM    = 'gcc'
	#EXEC_PATH = r'C:/Program Files/CodeSourcery/Sourcery G++ Lite/bin'
elif CROSS_TOOL == 'keil':
    PLATFORM 	= 'armcc'
	#EXEC_PATH = r'C:/Keil_v5'
elif CROSS_TOOL == 'iar':
	PLATFORM 	= 'iar'
	#EXEC_PATH = r'C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.0'

if os.getenv('RTT_EXEC_PATH'):
	EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = 'debug'
#EFM32_FAMILY = 'Gecko'
EFM32_FAMILY = 'Giant Gecko'
EFM32_TYPE = 'EFM32GG380F1024'

if PLATFORM == 'gcc':
	# toolchains
	PREFIX = 'arm-none-eabi-'
	CC = PREFIX + 'gcc'
	CXX = PREFIX + 'g++'
	AS = PREFIX + 'gcc'
	AR = PREFIX + 'ar'
	LINK = PREFIX + 'gcc'
	TARGET_EXT = 'elf'
	SIZE = PREFIX + 'size'
	OBJDUMP = PREFIX + 'objdump'
	OBJCPY = PREFIX + 'objcopy'
	STRIP = PREFIX + 'strip'
	
	DEVICE = ' -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections'
	CFLAGS = DEVICE + ' -Wall'
	AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
	LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map=rtthread-efm32.map,-cref,-u,__cs3_reset -nostartfiles -T'
	if EFM32_FAMILY == 'Gecko':
		LFLAGS += ' efm32g_rom.ld'
	elif EFM32_FAMILY == 'Giant Gecko':
		LFLAGS += ' efm32gg_rom.ld'

	CPATH = ''
	LPATH = ''

	if BUILD == 'debug':
		CFLAGS += ' -O0 -gdwarf-2'
		AFLAGS += ' -gdwarf-2'
	else:
		CFLAGS += ' -O2  -Os'

	POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
elif PLATFORM == 'armcc':
	# toolchains
	CC = 'armcc'
	CXX = 'armcc'
	AS = 'armasm'
	AR = 'armar'
	LINK = 'armlink'
	TARGET_EXT = 'axf'

	DEVICE = ' --device DARMSTM'
	CFLAGS = DEVICE + ' --apcs=interwork'
	AFLAGS = DEVICE
	LFLAGS = DEVICE + ' --info sizes --info totals --info unused --info veneers --list rtthread-efm32.map --scatter rtthread-efm32.sct'

	CFLAGS += ' -I' + EXEC_PATH + '/ARM/RV31/INC'
	LFLAGS += ' --libpath ' + EXEC_PATH + '/ARM/RV31/LIB'

	EXEC_PATH += '/arm/bin40/'

	if BUILD == 'debug':
		CFLAGS += ' -g -O0'
		AFLAGS += ' -g'
	else:
		CFLAGS += ' -O2 -Otime'

	POST_ACTION = 'fromelf --bin $TARGET --output rtthread.bin \nfromelf -z $TARGET'


