TARGET = gw_transmit_service

# sdk
SDK_DIR   = ../sdk-linux
SDK_BRANCH = master
ifeq ($(SDK_DIR), $(wildcard $(SDK_DIR)))
else
$(shell cd ../ && git clone -b $(SDK_BRANCH) git@github.com:leadercxn/sdk-linux.git)
endif

# 版本脚本
$(shell python2 $(SDK_DIR)/scripts/version/preBuild.py)
VERSION = $(shell python2 $(SDK_DIR)/scripts/version/getVersion.py)_$(shell git rev-parse --short HEAD)

# 时间戳
TIMESTAMP = $(shell date +%Y%m%d%H%M)


# LOG Message
$(info VERSION: $(VERSION))



CC = gcc
CFLAGS = -Wextra -Wall -std=gnu99 -Wno-unused-parameter -Wno-unused-variable -Wno-duplicate-decl-specifier


UNAME = $(shell uname -o)
ifeq ($(UNAME), Msys)
MSFLAGS = -lws2_32
endif

# C sources
C_SOURCES = 	\
src/mqtt.c		\
src/mqtt_pal.c	\
../sdk-linux/components/cJSON/cJSON.c	\
../sdk-linux/components/trace/trace.c

# C includes
C_INCLUDES =  		\
-Ibsp					\
-Isrc					\
-Iexamples/templates	\
-I../sdk-linux/components/cJSON	\
-I../sdk-linux/components/trace


CFLAGS += $(C_INCLUDES) $(C_DEFS)

# 生成目标文件
BINDIR = bin

EXEC_BIN  = $(BINDIR)/simple_publisher
EXEC_BIN += $(BINDIR)/openssl_publisher
EXEC_BIN += $(BINDIR)/bio_publisher
EXEC_BIN += $(BINDIR)/reconnect_subscriber 
EXEC_BIN += $(BINDIR)/simple_subscriber
EXEC_BIN += $(BINDIR)/$(TARGET)

# C 宏
C_DEFS =  \
-DBOARD_RASPBERRY4_V1_0 \
-DTRACE_ENABLE	\
-DTRACE_LEVEL=5

all: $(BINDIR) $(EXEC_BIN)

$(BINDIR)/simple_%: examples/simple_%.c $(C_SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -lm $(MSFLAGS) -o $@

$(BINDIR)/reconnect_%: examples/reconnect_%.c $(C_SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -lm $(MSFLAGS) -o $@

$(BINDIR)/bio_%: examples/bio_%.c $(C_SOURCES)
	$(CC) $(CFLAGS) `pkg-config --cflags openssl` -D MQTT_USE_BIO $^ -lpthread -lm $(MSFLAGS) `pkg-config --libs openssl` -o $@

$(BINDIR)/openssl_%: examples/openssl_%.c $(C_SOURCES)
	$(CC) $(CFLAGS) `pkg-config --cflags openssl` -D MQTT_USE_BIO $^ -lpthread -lm $(MSFLAGS) `pkg-config --libs openssl` -o $@

$(BINDIR)/$(TARGET): main.c $(C_SOURCES)
	$(CC) $(CFLAGS) $^ -lpthread -lm $(MSFLAGS) -o $@


$(BINDIR):
	mkdir -p $(BINDIR)


clean:
	rm -rf $(BINDIR)

