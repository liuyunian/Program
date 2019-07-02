include config.mk

all:   
# 用shell脚本的for循环来编译，shell脚本中的变量使用时要用两个$
	@for dir in $(BUILD_DIR); \
		do \
			make -C $$dir; \
		done

clean :
# -rf：删除文件夹，强制删除
	rm -rf app/link_obj app/dep nginx
	rm -rf signal/*.gch app/*.gch