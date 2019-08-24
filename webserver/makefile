CC = g++
CXXFLAGS = -lpthread -std=c++11
OBJS = http_connection.o server.o mimetype.o main.o
TARGET = webserver

$(TARGET) : $(OBJS)
    $(CC) -o $(TARGET) $(OBJS)
$(OBJS) : %.o:%.c
    $(CC) -c $< -o $@ $(CFLAGS)

clean:
	-$(RM) $(OBJS) (TARGET)