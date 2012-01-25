#include "disc_modules.h"

main()
{
  int ser_sock, cli_sock, ser_len, cli_len,fd1;
  size_t nread;
  char buf[BUFSIZ];
  char buf1[BUFSIZ];
  struct sockaddr_in ser, cli;
  
  if ((ser_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
  {
      fprintf(stderr, "Unable to create server socket\n");
      exit(1);
  }
  ser_len = sizeof(ser);     
  bzero((char *)&ser, ser_len);
  ser.sin_family      = AF_INET;
  ser.sin_port        = htons(SER_PORT);   
  ser.sin_addr.s_addr = htonl(INADDR_ANY);

  if ((bind(ser_sock, (struct sockaddr *)&ser, ser_len)) == -1) 
  {
      fprintf(stderr, "Unable to bind to service port for server\n");
      exit(2);
  }

  if (listen(ser_sock, LISTENQ)) 
  {
      fprintf(stderr, "Unable to create a client request queue\n");
      exit(3);            
  }
  
  cli_len = sizeof(cli);
  for (;;) 
  {
         cli_sock = accept(ser_sock, (struct sockaddr *)&cli, &cli_len);
         if (cli_sock == -1)
	 {
         	  fprintf(stderr, "accept() failed\n"); 
	          exit(4);
      	 }
      
      	 if(fork()==0)
	 {       
		char uname[20];
		char str_buf;
		close(ser_sock);
		nread = recv(cli_sock, uname,20, 0);  
		updateUser( uname , LOG_IN );
		updateLog ( uname , LOG_IN );
		while(1)
		{
		   showmenu(&cli_sock,uname);
		   bzero(buf,sizeof buf);
		   nread = recv(cli_sock, buf,10, 0);  /* read from client */
		   switch(buf[0])
		   {	
			case 'c' :
			case 'C' :
			{
				printf("inside server's create discussion\n");
                                createDisc( &cli_sock );
			}break;
		
			case 'd' :
			case 'D' :
			{
				printf("inside server's select discussion\n");
                                showDiscList( &cli_sock );
			}break;
			case 'o' :
			case 'O' :
			{
				printf("inside server's show online users\n");
                                showUsers( &cli_sock );
			}break;
			
			case 's' :
			case 'S' :
			{
				printf("inside server's show discussion\n");
				showDiscussion( &cli_sock );
			}break;

			case 'p' :
			case 'P' :
			{
				printf("inside server's post comment\n");
				postComment( &cli_sock ,uname);
				/*For(uname online in discussion)
				{	updateWall(socket, uname);
				}*/
				
			}break;

			default :
			{	
                		updateLog ( uname , LOG_OUT );
				updateUser( uname , LOG_OUT );
				close(cli_sock);
				exit(0);	
			}break;

		   }
		}
		close(cli_sock);  
	      exit(0);
	}
  }
  close(cli_sock);
  return(0);    
} 

