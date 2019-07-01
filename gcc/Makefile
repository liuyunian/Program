CC = gcc
CFLAGS = -Isub -Iadd
TARGET = cacu.out
OBJS = add/add_int.o add/add_float.o sub/sub_int.o sub/sub_float.o main.o

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJS) : %.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	$(RM) $(TARGET) $(OBJS)
