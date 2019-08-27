CC = g++
CXXFLAGS = -lpthread -std=c++11
OBJS = c_http_connection.o c_server.o main.o
TARGET = webserver

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJS) : %.o:%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	-$(RM) $(OBJS) $(TARGET)