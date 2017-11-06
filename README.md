# wechat_project
### 模仿微信的登录、注册、聊天、发红包做的简易项目。
### 基本功能介绍
1. 支持用户登录及注册
2. 支持好友管理，包括添加好友、删除好友(单方面删除、彻底删除）
3. 支持群管理，包括创建群、加入群、退群
4. 支持聊天功能，包括群聊和单聊
5. 支持传输数据（文字、图片、视频、音频）
6. 支持发红包，包括单发和群发红包，同时支持多人抢红包。
7. 支持处理红包过期退还功能
8. 支持用户查看红包历史记录

### 功能模块图：
![图3](https://github.com/zy943453722/wechat_project/功能模块.png)
### 数据库设计任务：

建库：1个----WeChat

建表：6个
1. 用户表(Users)
2. 好友表(Friends)
3. 群表(Groups)
4. 群成员表(Gmember)
5. 发红包表(RedPacket)
6. 红包记录表(RedPacketRecord)

建索引:1个----好友索引(u_friends)

#### 数据库设计：

***数据字典：***

![图1](https://github.com/zy943453722/wechat_project/数据字典.png)

***E-R图：***
![图2](https://github.com/zy943453722/wechat_project/E-R.png)
### 文件结构分布：
**.h文件:<br>**
* mysql.h (封装数据库)
* ser.h   (封装服务器基本连接）
* cli.h   (封装客户端基本连接)
* login.h  (封装登录功能基本函数)
* register.h(封装注册功能基本函数）
* friends.h(封装好友管理功能基本函数)
* group.h(封装群管理功能基本函数)
* chat.h (封装聊天功能基本函数)
* redpacket.h(封装红包功能基本函数)
***<br>.c文件:<br>***
- mysql.c
- ser.c
- cli.c
- login.c
- register.c
- friends.c
- group.c
- chat.c
- wechat_ser.c(封装最终服务器端处理逻辑）
- wechat_cli.c(封装最终客户端处理逻辑）
