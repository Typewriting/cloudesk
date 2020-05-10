#cloudesk网盘

## 简介

开发环境：Linux

开发工具：vim,gcc,gdb,make,mysql,github开源工具

功能：用户注册、登陆、查看、上传、下载、秒传、创建子目录、创建文件等

## 支持的客户端操作：

1. 新用户注册  new （在登陆界面输入)
2. 进入对应目录  cd name           
4. 将本地文件上传至服务器  puts filename     
5. 文件名 下载服务器文件到本地 gets filename     
6. 删除上传的文件/目录 remove            
7. 显示目前所在路径 pwd               
8. 在当前文件夹创建目录 mkdir name
9. 退出程序 exit

客户端在本地将指令转换成操作码，同时过滤无效操作，将操作码发送至服务器端，服务器端接收后，执行相应的操作。

## 工作描述：

1. 利用socket编程实现服务器和客户端的通信； 

socket：套接口。是操作系统内核中的一个数据结构，它是网络进程的ID。Linux 中的网络编程是通过 socket 接口来进行的。

通过socket编程实现TCP协议流程：

服务端：socket---bind---listen---while(1){---accept---recv---send---close---}---close
客户端：socket------------------------------connect---send---recv---------------close

各部分函数的作用：

+ socket 函数：生成一个套接口描述符。
+ bind 函数：用来绑定一个端口号和 IP 地址，使套接口与指定的端口号和 IP 地址相关联。
+ listen 函数：使服务器的这个端口和 IP 处于监听状态，等待网络中某一客户机的连接请求。如果客户端有连接请求，端口就会接受这个连接。
+ accept 函数：接受远程计算机的连接请求，建立起与客户机之间的通信连接。
+ recv函数：用新的套接字来接收远端主机传来的数据，并把数据存到由参数 buf 指向的内存空间。
+ send 函数：用新的套接字发送数据给指定的远端主机。
+ close 函数：当使用完文件后若已不再需要则可使用close()关闭该文件，并且close()会让数据写回磁盘，并释放该文件所占用的资源。
+ connect 函数：用来请求连接远程服务器，将参数 sockfd 的 socket 连至参数 serv_addr 指定的服务器 IP 和端口号上去。

2. 使用线程池响应多客服端的请求：

服务器创建多个子线程，让它们等待在条件变量上。若有客户端连接，由主线程把客户端的文件描述符放入队列（类似生产者消费者模型），同时用signal唤醒一个子进程，到队列中获取任务，响应客户端的操作。

3. 使用虚拟文件系统和mysql数据库来设计服务器的文件系统，节省数据库存储空间。

在MySQL数据库中建立文件表，表项分别为：

+ code 文件在表中的序号
+ precode 用于标识文件夹，若文件处于根目录，则为1
+ name 文件名
+ userid 用户id(保存在MySQL用户表里)
+ size 文件大小
+ md5 文件的md5值

文件夹和文件存储在同一MySQL表中，和文件的区别为：文件夹的md5为NULL。

不管用户存储文件的层次如何，服务器端的存储区都只有文件，不建立文件夹。用户创建文件夹，只是在MySQL表中新建一个表示此文件夹的表项。为避免重名，服务器端以code代替实际的文件名来存储文件。

4. 利用文件MD5码值保证传输的差错控制和文件秒传

用户执行puts命令上传文件，本地计算出此文件的md5码，将其同文件名、文件大小、所在文件夹，一同发送给服务器端，然后传送文件。服务器端接收到文件后，计算此文件的md5值，与客户端发送的md5值比较，若一致，则认为上传成功。在MySQL表中新建表项，以该表项code值命名该文件。

用户执行get命令下载文件时，把文件名发送给服务器端，服务器端若找到该用户名下有此文件，便执行发送程序，客户端接收到文件后，计算此文件的md5值，若与服务器端的相同，则认为下载成功，以文件的真实名称重命名该文件。

秒传的实现：

用户执行puts命令上传文件，先将计算得到的该文件的md5码传给服务器端，若服务器端在MySQL表中查到此md5码，则直接在MySQL表新建一个表项，userid和precode项依照客户端的用户名和当前文件夹。之后通知客户端上传成功。

5. 利用SHA-512加密算法保存用户密码

用户注册时输入密码后，在本地通过随机数生成一段salt值(8位)，通过调用crypt函数，将salt值和密码通过SHA-512加密算法生成一段字符串。这段字符串由salt值和加密后的密码拼接而成，将其和用户名上传到服务器端的MySQL表中保存。
登陆时，待用户输入用户名和密码后，将用户名传给服务器端，若能在MySQL表中找到此用户名，则将salt值和加密后的密码组成的字符串传回客户端，客户端拿到salt值后，将其和用户输入的密码一起执行SHA-512加密算法，得到的字符串和服务器端返回的比较，若一致，则进行登陆。

6. 记录服务器操作日志，存入数据库；





