#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#define MAXLINE 1024

ssize_t
writen(int fd, const void* vptr, size_t n)
{
    size_t  nleft;
    ssize_t nwrote;
    const char *ptr = NULL;

	ptr = vptr;
	nleft = n;
	while (nleft >0)
	{
	    if((nwrote = write(fd, vptr, n)) <= 0)
		{
		    if(nwrote<0 && errno == EINTR)
				nwrote = 0;
			else
				return -1;
		}
		nleft -= nwrote;
		ptr += nwrote;
	}
    return n;
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    size_t n, rc;
    char c, *ptr;
    
    ptr = vptr;
    for (n = 1; n < maxlen; n++)
    {
        again:
            if ((rc = read(fd, &c, 1)) == 1)
            {
                *ptr++ = c;
                if (c == '\n')
                    break;
            }
            else if (rc == 0)
            {
                *ptr = 0;
                return (n-1);
            }
            else
            {
                if(errno == EINTR)
                    goto again;
                return (-1);
            }
             
    } 

    *ptr = 0;
     return n;
}

void
str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE]; 
    char recvline[MAXLINE];
    int readbytes;
    int nwrote;
    while(fgets(sendline, MAXLINE, fp) != NULL)
    {
	writen(sockfd, sendline, strlen(sendline));
	if((readbytes = readline(sockfd, recvline, MAXLINE)) <= 0)
	{
            // abort();
            printf("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
	}
	fputs(recvline, stdout);
        if(strncmp(sendline, "quit", 4) == 0)
        {
            shutdown(sockfd, SHUT_WR);
            
        }
    }
}

int main(int argc, char **argv)
{
    int sockfd, ret, sndbytes, readbytes;
    struct sockaddr_in serv_addr;
    
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
    bzero(&serv_addr, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
    
    ret = connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(ret < 0)
    {
        printf("connect error: %s\n", strerror(errno));
        abort();
    }

    str_cli(stdin, sockfd);
    exit(0);
}
