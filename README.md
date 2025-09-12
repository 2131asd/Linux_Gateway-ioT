

Linux codes

=======
# Linux物联网网关（多线程实现）
Linux多线程方式实现嵌入式网关Server。  
   应用于物联网嵌入式项目
 包括参数数据解析、协议转换、Socket收发、Sqlite、Uart、Camera等操作&UI界面。
 
 （bin里有编译好的sqlite3(6410) lib里有库文件编译用。）

第一步：阅读README.md
先了解项目的整体定位（多线程物联网网关）、核心功能（参数解析、协议转换、Socket、SQLite、硬件控制等）、编译 / 运行提示（如bin和lib的作用），建立对项目的 “全局认知”。

第二步：分析main.c
作为程序入口，main.c决定了 “程序如何启动”：哪些模块被初始化、哪些线程被创建、执行流程是怎样的。从这里能理清项目的顶层逻辑。

第三步：梳理data_global.c / data_global.h
全局数据是各模块（线程）交互的核心。理解了全局变量、数据结构，才能明白 “线程间如何共享 / 传递数据”。

第四步：研究Makefile
编译脚本决定了 “代码如何变成可执行程序”：哪些文件参与编译、依赖哪些库、链接顺序是什么。这一步能明确文件间的依赖关系，也为后续本地编译 / 调试做准备。

第五步：解析基础数据结构（link_list.c / link_list.h、sqlite_link_list.c / sqlite_link_list.h）
链表是很多功能模块的基础支撑（如缓存队列、数据库结果遍历）。先掌握基础数据结构，再分析上层功能会更清晰。

第六步：逐个分析pthread_*.c线程模块
按 “核心通信 → 数据处理 → 硬件 / 外设控制” 的逻辑顺序分析：
核心通信：pthread_client_receive.c → pthread_client_send.c → pthread_client_request.c（网络数据收发，是网关与外部交互的核心）。
数据处理：pthread_analysis.c → pthread_transfer.c → pthread_sqlite.c（数据解析、协议转换、数据库存储，是网关的 “大脑”）。
硬件 / 外设控制：pthread_uart_cmd.c → pthread_camera.c → pthread_led.c → pthread_buzzer.c → pthread_infrared.c → pthread_sms.c → pthread_heart_jump.c → pthread_refresh.c（与硬件交互，是网关的 “四肢”）。
每个线程模块可结合data_cache.c / data_cache.h分析（缓存是线程间数据传递的常见 “中间层”）。

第七步：补充分析辅助组件
最后看sem.h（线程同步机制，理解多线程如何互斥访问共享资源）、sqlite3.h（数据库 API，辅助分析pthread_sqlite.c），以及bin/lib中的二进制 / 库文件（验证编译结果）。

三、为什么按这个顺序分析？
这个顺序遵循 **“从整体到局部、从基础到核心、从依赖到功能”** 的逻辑，能最大程度降低理解难度：

先读README：避免一开始就陷入代码细节，先建立 “全局地图”。
再看main.c：从程序入口把握 “顶层流程”，知道各个模块的启动顺序。
全局数据与编译脚本：是项目的 “骨架”（数据交互方式 + 编译规则），先理清骨架，再填肉（功能模块）。
基础数据结构先行：链表等是很多功能的 “地基”，先掌握地基，再看上层建筑（线程功能）更顺畅。
线程模块分层次分析：从 “核心通信” 到 “外设控制”，符合物联网网关 “数据流入→处理→输出 / 控制” 的业务逻辑，也能逐步理解多线程如何协作完成复杂任务。
辅助组件最后补充：信号量、数据库 API 等是 “工具类”，在分析线程同步、数据库操作时再深入，更具针对性。
