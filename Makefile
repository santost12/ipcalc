CC = gcc
CCFLAGS = -O2 -static -Wall -Werror
CCFLAGS_DEBUG = -Og -g -Wall -Werror

TARGET = ipcalc
TARGET_DEBUG = ipcalc_debug

all: release debug

release:
	$(CC) $(CCFLAGS) ipcalc.c -o $(TARGET)

debug:
	$(CC) $(CCFLAGS_DEBUG) ipcalc.c -o $(TARGET_DEBUG)

clean:
	$(RM) $(TARGET)
	$(RM) $(TARGET_DEBUG)

