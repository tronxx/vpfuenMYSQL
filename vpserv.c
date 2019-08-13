/*
	vpserv.c General Purpose SERVER
	it works by receiving requests from
	clients. The request structure:
	
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#define MP while(*ptr++ != ',')

void err (char *,...);
void *getad(char *ptr);
int valu(char *ptr);

void vCliSer(char *ptr)
{
  int sockfd, client_sockfd,child;
  int nread, len, max;
  char action,*buf;
  struct sockaddr_in serv_addr, client_addr;
  errno = 0;

  action = *ptr;

 switch(action) {

  case 's': /* socket Js,lSock,; create a socket */
   if ((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0) {
     err ("socket() failed. errno = %d,%s", errno,strerror(errno));
   }
   MP; *(int*)getad(ptr) = sockfd;
  return;

  case 'b': /* bind Jb,lPort,lsocket,; bind with port PP */
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   MP;
   serv_addr.sin_port = htons(valu(ptr));
   MP;
   sockfd = *(int*)getad(ptr);
   if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
     err ("bind() failed. errno = %d,%s", errno,strerror(errno));
   }
  return;

  case 'l': /* listen Jl,lSock,iNum,; */
   MP; sockfd = *(int*)getad(ptr);
   MP; 
   listen(sockfd, valu(ptr));
  return;

  case 'a': /* accept Ja,lSock,; */
   MP; sockfd = *(int*)getad(ptr);
   for (;;) {
    len = sizeof(client_addr);
    client_sockfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
    if (client_sockfd == -1) {
      continue;
    }
    if ((child = fork()) < 0) {
	err("Can't fork child (child = %d)",child);
    }
    else if(child == 0) {
	close(sockfd);
        *(int*)getad(ptr) = client_sockfd;
        return;
    }
    close(client_sockfd);
   }
  return;

  case 'A': /* JA,lSockSer,lSockCli,; */
   MP; sockfd = *(int*)getad(ptr);
   for (;;) {
    len = sizeof(client_addr);
    client_sockfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
    if (client_sockfd == -1) {
      continue;
    }
    else break;
   }
   MP; *(int*)getad(ptr) = client_sockfd;
  return;

  case 'c': /* close Bc,lSock,; */
   MP; sockfd = *(int*)getad(ptr);
   close(sockfd);
  return;

  case 'r':  /* read Br,lSock,sBuf,lMax,len,; */
   MP; sockfd = *(int*)getad(ptr);
   MP; buf = (char*)getad(ptr);
   MP; max = valu(ptr);
   len = read(sockfd,buf,max);
   MP; *(int*)getad(ptr) = len;
  return;

  case 'w':  /* write Bw,lSock,sBuf,lMax,len,; */
   MP; sockfd = *(int*)getad(ptr);
   MP; buf = (char*)getad(ptr);
   MP; max = valu(ptr);
   len = write(sockfd,buf,max);
   MP; *(int*)getad(ptr) = len;
  return;

  case 'C': /* connect BC,"192.168.0.1",2828,lSock,; */
   serv_addr.sin_family = AF_INET;
   MP; serv_addr.sin_addr.s_addr = inet_addr((char*)getad(ptr));
   MP; serv_addr.sin_port = htons(valu(ptr));
   MP; sockfd = *(int*)getad(ptr);
   if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
     err ("connect() failed. errno = %d,%s", errno,strerror(errno));
   }
  return;

 } /* End of switch(action) */

} /* end of functions */


