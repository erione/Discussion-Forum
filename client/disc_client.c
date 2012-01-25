#include "disc_modules.h"

main(int argc, char *argv[])
{

  int cli_sock, ser_len, port_number;
  size_t nread;
  char buf[BUFSIZ];
  char buf1[BUFSIZ];
  struct sockaddr_in  ser;
  struct hostent *ser_info;
  static struct sigaction act;
  pthread_t thread;
  if ((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
      fprintf(stderr, "Unable to create socket for client\n");
      exit(1);
  }
  ser_len = sizeof(ser);           
  bzero((char *)&ser, ser_len);    
  ser.sin_family      = AF_INET;   
  ser.sin_port = htons(SER_PORT);  

  if ((ser_info = gethostbyname(argv[1])) == (struct hostent *)NULL)
  {
      fprintf(stderr, "unknown server\n");
      exit(2);
  }
  bcopy((char *)ser_info->h_addr, (char *)&ser.sin_addr, ser_info->h_length);

  if ((connect(cli_sock, (struct sockaddr *)&ser, ser_len)) == -1)
  {
      fprintf(stderr, "can't connect to  server\n");
      exit(3);
  }
  bzero(buf,sizeof buf);
  do
  {
  printf("\n");
  printf("Enter your username : ");
  gets(buf);
  }
  while(buf[0]==0);
  send(cli_sock, buf, 20, 0);
  while(1)
  {
  recv(cli_sock,buf,500,0);
  puts(buf);
  bzero(buf,sizeof buf);
  gets(buf);
  send(cli_sock, buf, 10, 0);  
  switch(buf[0])
  {
  	case 'c' :
	case 'C' :
	{
		bzero(buf,sizeof buf);
		printf("\n");
             	printf("Enter name of discussion to be created  ( press enter to send )  :  ");
		gets(buf);
                send( cli_sock, buf, 30, 0 );
		recv( cli_sock,buf, 25, 0);
		if(fork()==0)
                {
                        execlp("clear",(char *)0,(char *)0);
                        exit(0);
                }
                else wait(0);
		puts( buf );
		printf("\n");
	}break;	

	case 'd' :
	case 'D' :
	{
		bzero(buf, sizeof buf);
		nread = recv(cli_sock, buf, 1000, 0);
                if(fork()==0)
                {
                        execlp("clear",(char *)0,(char *)0);
                        exit(0);
                }
                else wait(0);
		
		if(strcmp(buf,"No discussion has been created yet\n") == 0)
		{
			 puts( buf );
	                 printf("\n");
		}
		else
		{
		printf("\n#####   Discussions List   #####\n\n");
		puts( buf );
                printf("\n");
		printf("Enter Discussion no. : ");
		bzero(buf, sizeof buf);
		gets( buf );	
		send( cli_sock, buf, 10, 0);
		bzero( buf, sizeof buf);
		recv( cli_sock, buf, 20, 0);
		if(fork()==0)
                {
                        execlp("clear",(char *)0,(char *)0);
                        exit(0);
                }
		else wait(0);
		puts( buf );
		printf("\n");
		}
	}break;

	case 'o' :
	case 'O' :
	{	
		bzero(buf, sizeof buf);
		recv( cli_sock, buf, 500, 0);	
		if(fork()==0)
                {        
                        execlp("clear",(char *)0,(char *)0);
                        exit(0);
                }
                else wait(0);

                printf("\n#####  Online Users #####\n\n");
		puts(buf);
		printf("\n#########################\n\n");
	}break;
	case 's' :
	case 'S' :
	{
		bzero(buf, sizeof buf);
		nread = recv(cli_sock, buf, 2000, 0);
		if(fork()==0)
                {        
                        execlp("clear",(char *)0,(char *)0);
                        exit(0);
                }
                else wait(0);
		puts( buf );
		printf("\n");
	
	}break;
	
	case 'p' :
	case 'P' :
	{
		bzero(buf,sizeof buf);
		bzero(buf1,sizeof buf1);	
		recv(cli_sock,buf,25,0);
		if(strcmp(buf,"OK")!=0)
		{	
			if(fork()==0)
                        {
                                execlp("clear",(char *)0,(char *)0);
                                exit(0);
                        }
                        else wait(0);

			puts(buf);
		}
		else
		{
			printf("Enter post ( CTRL + R to send ) :\n");
			bzero(buf,sizeof buf);
			do
			{
				gets(buf);
				strcat(buf,"\n");
				strcat(buf1,buf);
			}while( *buf != '\n' );
			send( cli_sock, buf1, 500, 0 );
			nread = recv(cli_sock, buf, 25, 0);
			
			if(fork()==0)
		        {
			        execlp("clear",(char *)0,(char *)0);
				exit(0);
			}
        		else wait(0);
			puts( buf );
			printf("\n");		
		}
	}break;
	
	default : 
 		return;
  }
  }

  shutdown(cli_sock, SHUT_RDWR);     
  close(cli_sock);
}
