BUILD_ROOT = $(shell dirname $$PWD)
CC = g++
CXXFLAGS = -I $(BUILD_ROOT) -std=c++11
TARGET = client

LINK_OBJ_DIR = $(BUILD_ROOT)/app/link_obj
OBJS = $(LINK_OBJ_DIR)/ngx_c_conf.o \
		$(LINK_OBJ_DIR)/ngx_log.o \
		$(LINK_OBJ_DIR)/ngx_net_func.o \
		client.o

OBJS_GEN = client.o

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CXXFLAGS)

clean:
	-$(RM) $(TARGET) $(OBJS_GEN)