#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <bits/sigaction.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

#ifdef HAVE_POLL_H
#include <pool.h>
#endif

#ifdef HAVE_SYS_EVENT_H
#include <sys/event.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctrl.h>
#endif

#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_NET_IF_DL_H
#include <net/if_dl.h>
#endif

#ifdef HAVE_NETINET_SCTP_H
#include <netinet/sctp.h>
#endif

#define MAXLINE             4096    /* max text line length */
#define BUFFSIZE            8192    /* buffer size for reads and writes */

#define LISTENQ             1024    /* 2nd argument to listen() */
#define SERV_PORT           9877    /* TCP and UDP */
#define SERV_PORT_STR       "9877"  /* TCP and UDP */
#define UNIXSTR_PATH        "/tmp/unix.str"     /* Unix domain stream */
#define UNIXDG_PATH         "/tmp/unix.dg"      /* Unix domain datagram */

#define SA struct sockaddr

typedef void Sigfunc(int);      /* for signal handlers */

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

/* prototypes for our socket wrapper functions: see {Sec errors} */
int     Accept(int, SA *, socklen_t *);
void    Bind(int, const SA *, socklen_t);
void    Connect(int, const SA *, socklen_t);
void    Getpeername(int, SA *, socklen_t *);
void    Getsockname(int, SA *, socklen_t *);
void    Getsockopt(int, int, int, void *, socklen_t *);
#ifdef  HAVE_INET6_RTH_INIT
int     Inet6_rth_space(int, int);
void    * Inet6_rth_init(void *, socklen_t, int, int);
void    Inet6_rth_add(void *, const struct in6_addr *);
void    Inet6_rth_reverse(const void *, void *);
int     Inet6_rth_segments(const void *);
struct in6_addr *Inet6_rth_getaddr(const void *, int);
#endif
#ifdef	HAVE_KQUEUE
int     Kqueue(void);
int     Kevent(int, const struct kevent *, int,
            struct kevent *, int, const struct timespec *);
#endif
void    Listen(int, int);
#ifdef  HAVE_POLL
int     Poll(struct pollfd *, unsigned long, int);
#endif
ssize_t Readline(int, void *, size_t);
ssize_t	Readn(int, void *, size_t);
ssize_t Recv(int, void *, size_t, int);
ssize_t Recvfrom(int, void *, size_t, int, SA *, socklen_t *);
ssize_t Recvmsg(int, struct msghdr *, int);
int     Select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
void    Send(int, const void *, size_t, int);
void    Sendto(int, const void *, size_t, int, const SA *, socklen_t);
void    Sendmsg(int, const struct msghdr *, int);
void    Setsockopt(int, int, int, const void *, socklen_t);
void    Shutdown(int, int);
int     Sockatmark(int);
int     Socket(int, int, int);
void    Socketpair(int, int, int, int *);
void    Writen(int, void *, size_t);

/* lib */
const char * Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
void Inet_pton(int family, const char *strptr, void *addrptr);

/* log */
void    err_dump(const char *, ...);
void    err_msg(const char *, ...);
void    err_quit(const char *, ...);
void    err_ret(const char *, ...);
void    err_sys(const char *, ...);

/* stdio */
void    Fclose(FILE *fp);
FILE    * Fdopen(int fd, const char *type);
char    * Fgets(char *ptr, int n, FILE *stream);
FILE    * Fopen(const char *filename, const char *mode);
void    Fputs(const char *ptr, FILE *stream);