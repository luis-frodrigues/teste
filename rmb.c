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

typedef struct serv{
	char *name;
	char *ip;
	char *udp;
	char *tcp;
}serv; 


int main(){

int out=1, addrlen=0,msg_lenght=0;

char terminal[256];

char *server="show_servers";
char *publish="publish";
char *exitt="exit";
char *hbyname="tejo.tecnico.ulisboa.pt";
char buffer[256];
char msg[256]=" a;b;c;d";
char hostname[128];
struct hostent *host;
struct in_addr *inaddr;
struct sockaddr_in idserveraddr;
struct serv **servers=(struct serv**)malloc(sizeof(serv*));
char a='\0';
char x[128], z[128],c[128],v[128];
char strin[20][128];
int fd,n,j=0, o=0, u=0;

//malloc dos ponteiros da estrutura serv
for (int i = 0; i <=20; ++i)
{
	servers[i]=(struct serv*)malloc(sizeof(serv));

}


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
		//lê uma linha com um máximo de 140 carateres que é o máximo que a mensagem pode ter
		scanf("%140s",buffer);
		msg_lenght=strlen(buffer);

		//envia a mensagem, neste momento está para o tejo mas tem que ser para um server de msg
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
			//escreve os serves no terminal
			write(1,msg,n);
			

			//tentativa de meter o caracter '\0'
			sprintf(msg, "%s%c", msg,a);
			//msg[strlen(msg)]='\0';
			printf("mensagem: %s", msg );

			//inicializar todos os parâmetros para funcionar
			o=0; j=0; u=0;
			
			//Saber em que índice é que 'SERVERS\n' termina
			while(msg[o]!='\n')
			{
				o++;
			}
				
			/*retira os ';' e substitui os espaços para poder utilizar o sscanf
				e cada parágrafo é introduzido num respectivo indíce do vector de string */
			for (int i = o+1; i < (strlen(msg)-1); ++i)
			{
				if(msg[i]=='\n')
				{
					strin[j][u]='\0';
					i++;
					j++;
					u=0;	
				}else{
					if(msg[i]==';')
					{	
						msg[i]=' ';
					}
					strin[j][u]=msg[i];
					u++;
				}	
			}

			for (int i = 0; i < j; ++i)
			{
				printf("%d: %s\n",i,strin[i] );
				//divido os 4 parâmetros da string em 4 strings diferentes
				if(sscanf(strin[i],"%s %s %s %s",x,c,z,v )==EOF)
				printf("error: %s\n",strerror(errno));
				
				strcpy(servers[i]->name, x);
				strcpy(servers[i]->ip, c);
				strcpy(servers[i]->udp, z);
				strcpy(servers[i]->tcp, v);

				//Verificar se os parâmetros ficam na estrutura
				printf("name:%s\n ip:%s\n udp:%s\n tcp:%s\n",servers[i]->name,servers[i]->ip,servers[i]->udp,servers[i]->tcp );
			}
			
		}else{

			if(strcmp(terminal,exitt)==0)
				out=0;
		}
		printf("Estou no while de fora\n" );
	}
		
	
}
printf("first:%s\n second:%s\n", terminal, buffer);

	exit(0);
}