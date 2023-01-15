.PHONY: all clean pgmopts
all: drivebench

BUILD_REVISION := $(shell git describe --abbrev=10 --dirty --always --tags)
INSTALL_PREFIX := /usr/local/
CFLAGS := -Wall -Wextra -Wshadow -Wswitch -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Werror=implicit-function-declaration -Werror=format -Wno-unused-parameter
CFLAGS += -O3 -std=c11 -pthread -D_POSIX_SOURCE -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=500 -D_GNU_SOURCE -DBUILD_REVISION='"$(BUILD_REVISION)"'
CFLAGS += `pkg-config --cflags json-c`
CFLAGS += -ggdb3 -DDEBUG
CFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak -fsanitize=signed-integer-overflow
PYPGMOPTS := ../Python/pypgmopts/pypgmopts

LDFLAGS := `pkg-config --libs json-c`
TEST_PREFIX := local

OBJS := \
	argparse.o \
	diskinfo.o \
	drivebench.o \
	jsonwriter.o \
	md5.o \
	pgmopts.o \
	prng.o \
	seektime.o \
	semaphore.o \
	throughput.o

pgmopts:
	$(PYPGMOPTS) parser.py

install: all
	strip drivebench
	cp drivebench $(INSTALL_PREFIX)sbin/
	chown root:root $(INSTALL_PREFIX)sbin/drivebench
	chmod 755 $(INSTALL_PREFIX)sbin/drivebench

clean:
	rm -f $(OBJS) $(OBJS_CFG) drivebench

test: drivebench
	#./drivebench -vv --no-sequential --no-single-threaded-4k /dev/nvme0n1
	./drivebench -vv -j nvme.json /dev/nvme0n1
	#./drivebench -v -j out.json -i 1 -c 32 -r 30000 /dev/loop19

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

drivebench: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
