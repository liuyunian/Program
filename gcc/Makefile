cacu:add_int.o add_float.o sub_int.o sub_float.o main.o
	gcc add/add_int.o add/add_float.o sub/sub_int.o sub/sub_float.o main.o -o cacu

add_int.o:add/add_int.c add/add.h
	gcc -c add/add_int.c -o add/add_int.o

add_float.o:add/add_float.c add/add.h
	gcc -c add/add_float.c -o add/add_float.o 
 
sub_int.o:sub/sub_int.c sub/sub.h
	gcc -c sub/sub_int.c -o sub/sub_int.o
 
sub_float.o:sub/sub_float.c sub/sub.h
	gcc -c sub/sub_float.c -o sub/sub_float.o

main.o:add/add.h sub/sub.h
	gcc -c main.c -Iadd -Isub -o main.o

clean:
	rm -f cacu.out add/add_int.o add/add_float.o sub/sub_int.o sub/sub_float.o main.o