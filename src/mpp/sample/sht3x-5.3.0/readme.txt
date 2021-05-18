//readme

1.编译server端：
	ubuntun linux下编译:gcc server.c -o server(已测试)
	hisi linux下编译:arm-hisiv300-linux-gcc server.c -o server(还未测试)
	
	ubuntun linux运行(进入该目录):./server 127.0.0.1
	
	
2.编译client端，同时还要采集sht3x数据，故再ubuntu linux下运行时，需要屏蔽这些代码（68行-90行）
	ubuntun linux下编译:gcc sht3x_example_usage.c -o client(已测试)
	hisi linux下编译:make cc=arm-hisiv300-linux(还未测试)
	
	hisi linux运行需要mnt或者直接cpy到板子上（进入该目录）:./sht3x_example_usage 127.0.0.1

