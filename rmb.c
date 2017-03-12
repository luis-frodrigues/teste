#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
extern int errno;

typedef struct{
	char host[128];
	char *ip;
	int udp;
	int tcp;
}serv; 


int main(){

int out=1, addrlen=0,msg_lenght=0;

char terminal[256];

char *server="show_servers";
char *publish="publish";
char *exitt="exit";
char *hbyname="tejo.tecnico.ulisboa.pt";
char buffer[256];
char msg[256];
char hostname[128];
struct hostent *host;
struct in_addr *inaddr;
struct sockaddr_in idserveraddr;
serv servers[20];

int fd,n;

//user host name
if(gethostname(hostname,128)==-1)
	printf("error: %s\n",strerror(errno));

else printf("hostname:%s ", hostname);

//tecnico host
if((host= gethostbyname(hbyname))==NULL)
	printf("error: %s\n",strerror(errno));
else
	printf("official host name: %s\n",host->h_name);

//puts the address of tecnico in a different srtucture
inaddr=(struct in_addr*)host->h_addr_list[0];
printf("internet address: %s (%08lX)\n",inet_ntoa(*inaddr),(long unsigned int)ntohl(inaddr->s_addr));

//socket for UDP connection
fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
if(fd==-1)
{
	printf("error: %s\n",strerror(errno));
	exit(1);//error
}
	

//set the ip and port of the ID Server to a sockaddr_in structure
memset((void*)&idserveraddr,(int)'\0',sizeof(idserveraddr));
idserveraddr.sin_family=AF_INET;
idserveraddr.sin_addr= *inaddr;
idserveraddr.sin_port=htons(59000);

addrlen=sizeof(idserveraddr);
//while in which is read the keybord commands
while(out)
{
	scanf("%s", terminal);
	/*Coloca a mensagem na string buffer*/
	if(strcmp(terminal,publish)==0)
	{
		scanf("%140s",buffer);
		msg_lenght=strlen(buffer);

		printf("cenas:%s %d \n", buffer, msg_lenght );
		sendto(fd, buffer, msg_lenght,0, (struct sockaddr*)&idserveraddr,addrlen);
	
		n=recvfrom(fd, msg, sizeof(msg),0, (struct sockaddr*)&idserveraddr,(socklen_t*)&addrlen);
		if(n==-1)
			printf("error: %s\n",strerror(errno));

		write(1,msg,n);
		

	}else{
		//se foi escrito show_servers, envia GET_SERVERS ao ID Server e devolve a lista dos servidores
		if(strcmp(terminal,server)==0)
		{
			
			sendto(fd, "GET_SERVERS", 11,0, (struct sockaddr*)&idserveraddr,addrlen);
			if(n==-1)
				printf("error: %s\n",strerror(errno));

			n=recvfrom(fd, msg, sizeof(msg),0, (struct sockaddr*)&idserveraddr,(socklen_t*)&addrlen);
			if(n==-1)
				printf("error: %s\n",strerror(errno));
	
			write(1,msg,n);
			sscanf(msg,"%s;%s;%d;%d",servers[0].host,servers[0].ip,&(servers[0].udp),&(servers[0].tcp));
		}else{

			if(strcmp(terminal,exitt)==0)
				out=0;
		}
		printf("damn\n" );
	}
		
	
}
printf("first:%s\n second:%s\n", terminal, buffer);
printf("%s;%s;%d;%d",servers[0].host,servers[0].ip,servers[0].udp,servers[0].tcp);


	exit(0);
}