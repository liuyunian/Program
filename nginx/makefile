include config.mk

# 用shell脚本的for循环来编译，shell脚本中的变量使用时要用两个$
# make -C dir或者 -directory=dir，将dir目录作为当前目录
all:   
	@for dir in $(BUILD_DIR); \
		do \
			make -C $$dir; \
		done

clean :
	rm -rf app/link_obj app/dep nginx
	rm -rf signal/*.gch app/*.gch