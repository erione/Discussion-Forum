#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define LOG_IN 0
#define LOG_OUT 1
#define POST 2
#define  SER_PORT  49495
#define  LISTENQ 3


char c_time[50];
char temp1[20];
char discno[10];
int cnt,count=0;
char discname[30];

char * currDisc(void);
int getFileSize( FILE * file )
{
        fseek(file,0L,SEEK_END);
        int sz = ftell( file );
        rewind( file );
        return sz;
}

void showmenu(int *client_socket, char * uname )
{
	char buf[BUFSIZ];
	bzero(buf,sizeof buf);
	strcat(buf,"###########   Computer Talks   ###########\n\n");
	
	strcat(buf,"C - Create New Discussion\t\tUser : ");
	strcat(buf, uname );
	strcat(buf,"\n");
	strcat(buf,"D - Select Discussion\t\t\tCurrent Discussion : ");	
	strcat(buf, currDisc() );
        strcat(buf,"O - Show online Users\n");
	strcat(buf,"S - Show Discussion Board\n");
        strcat(buf,"P - Post Comment\n");
        strcat(buf,"E - Exit\n");
        strcat(buf,"Enter your choice : \n");
	puts(buf);
	send(*client_socket,buf,500,0);
}

char * showTime(void)
{
        time_t now;
        time(&now);
        strcpy(c_time,(char *)ctime(&now));
        return c_time;
}

int getCount( void )
{
        cnt=0;
        char buf[128];
	bzero(buf,sizeof buf);
        FILE * file;
        file = fopen("disclist.txt","a+");
        struct flock f_lock;
	f_lock.l_type = F_RDLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
        while( fgets(buf,128,file) != NULL )
        {
                cnt++;
        }
        f_lock.l_type = F_UNLCK;
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not unlocked\n");
                exit(1);
        }
        printf("File unlocked\n");
	bzero(buf,sizeof buf);
        return cnt;
}

char * currDisc( void )
{
	char buf[30];
	int i;
	bzero(buf,sizeof buf);
	if(count==0)
	{
		return "No Discussion Selected\n";
	}
	else
	{
		FILE * file;
	        file = fopen( "disclist.txt", "a+");
		for(i=0;i<count;i++)
		{
			fgets(buf,30,file);
			bzero(discname,sizeof discname);
		        strcpy(discname,buf);
	        }
	fclose(file);
	return discname;
	}
}
void createDisc( int * client_socket )
{
	char buf[BUFSIZ],buf1[10];
	bzero(buf,sizeof buf);
	bzero(buf1,sizeof buf1);
	FILE * file;
	file = fopen( "disclist.txt", "a+");
	recv( *client_socket, buf, 30, 0);
	strcat(buf,"\n");
        struct flock f_lock;
	f_lock.l_type = F_WRLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
        fputs( buf, file);
	f_lock.l_type = F_UNLCK;
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not unlocked\n");
                exit(1);
        }
        printf("File unlocked\n");
	fclose(file);
	sprintf(buf1,"%d",getCount());
	bzero(buf,sizeof buf);
	strcpy(buf,"disc");
	strcat(buf,buf1);
	file = fopen( buf,"a+");
	fclose(file);
	bzero( buf, sizeof buf);
        strcpy(buf,"Discussion Created\n\n");
	puts(buf);
        send( *client_socket,buf,25,0);		
	bzero(buf,sizeof buf);
	bzero(buf,sizeof buf1);

}

void showDiscList( int * client_socket )
{
	char buf1[BUFSIZ],buf[BUFSIZ];
        bzero(buf, sizeof buf);
        bzero(buf1, sizeof buf1);
	int i=1;
        const char filename[] = "disclist.txt";
        FILE * file = fopen(filename,"a+");
        struct flock f_lock;
        f_lock.l_type = F_RDLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
	char temp[10];
        while( fgets(buf,128,file) != NULL )
        {	
		sprintf(temp,"%2d",i);	
		strcat(buf1,temp);
		strcat(buf1," : ");
                strcat(buf1,buf);
		i++;
        }
	i=1;
	if(strcmp(buf1,"")==0)
                send(*client_socket,"No discussion has been created yet\n",35,0);
        else
        {
		send(*client_socket,buf1,1000,0);
        	f_lock.l_type = F_UNLCK;
	        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
	        {
	                perror("File not unlocked\n");
	                exit(1);
	        }
	        printf("File unlocked\n");
		bzero(buf1,sizeof buf1);
		recv(*client_socket, buf1, 10, 0);
		if( atoi(buf1) > 0 && atoi(buf1) <= getCount() )
		{
			count=atoi(buf1);
			bzero(discno,sizeof discno);
			strcpy(discno,"disc");
			strcat(discno,buf1);
	        	puts(discno);
			bzero(buf,sizeof buf);
			strcpy( buf, "Discussion Selected\n");
			puts(buf);
			send(*client_socket,buf,20,0);	
		}
		else
		{
			printf("%d\n",atoi(buf1));
			bzero(buf,sizeof buf);
	                strcpy( buf, "Choice invalid\n\n");
			puts(buf);
	                send(*client_socket,buf,20,0);
	
		}
		bzero(buf,sizeof buf);
	
	}
}
	
void showUsers( int * client_socket )
{
	char buf1[BUFSIZ],buf[BUFSIZ];
        bzero(buf, sizeof buf);
        bzero(buf1, sizeof buf1);
        const char filename[] = "users.txt";
        FILE * file = fopen(filename,"r");
        struct flock f_lock;        
	f_lock.l_type = F_RDLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
        while( fgets(buf,128,file) != NULL )
        {
	        strcat(buf1,buf);
	}
	puts(buf1);
        send(*client_socket,buf1,500,0);
        f_lock.l_type = F_UNLCK;
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not unlocked\n");
                exit(1);
	}
	printf("File unlocked\n");
	bzero(buf,sizeof buf);
	bzero(buf1,sizeof buf1);

}
void updateUser( char * uname , int flag )
{
	char str_buf[21],buf[2000];
	bzero(str_buf, sizeof str_buf );
	bzero(buf,sizeof buf);
	const char filename[] = "users.txt";
	FILE * file = fopen( filename,"a+" );
        if(file == NULL)
        {
	        printf("Cannot open users file.\n");
                exit(1);
        }
        struct flock f_lock;
	f_lock.l_whence = SEEK_SET;
	f_lock.l_start = 0;
	f_lock.l_len = getFileSize( file );
	int fd = fileno( file );
	
	if( flag == LOG_IN )
	{
		f_lock.l_type = F_WRLCK;
	        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
	        {
        	        perror("File not locked\n");
                	exit(1);
	        }
		printf("File locked\n"); 
		strcpy(str_buf,uname);
		strcat(str_buf , "\n");
		fputs( str_buf, file );

		f_lock.l_type = F_UNLCK;
	        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
	        {
	                perror("File not unlocked\n");
	                exit(1);
	        }
		printf("File unlocked\n");
		fclose(file);
	}
	else if( flag == LOG_OUT )
	{	
		f_lock.l_type = F_RDLCK;
                if( fcntl(fd,F_SETLKW, &f_lock) == -1)
                {
                        perror("File not locked\n");
                        exit(1);
                }
                printf("File locked\n");
		
		bzero( str_buf, sizeof str_buf );
		bzero( temp1, sizeof temp1);
		strcpy( temp1, uname );
		strcat( temp1,"\n");
		while( fgets(str_buf,128,file) != NULL )
	        {      
			 puts(temp1);
			if(strcmp(str_buf,temp1) != 0)
			{
				strcat(buf,str_buf);
				bzero(str_buf,sizeof str_buf);
			}	
		}
		f_lock.l_type = F_UNLCK;
                if( fcntl(fd,F_SETLKW, &f_lock) == -1)
                {
                        perror("File not unlocked\n");
                        exit(1);
                }
                printf("File unlocked\n");
                fclose(file);
		file = fopen("users.txt","w+");
		f_lock.l_type = F_WRLCK;
                if( fcntl(fd,F_SETLKW, &f_lock) == -1)
                {
                        perror("File not locked\n");
                        exit(1);
                }
                printf("File locked\n");
		
		fd = fileno( file );
		fputs(buf,file);
		f_lock.l_type = F_UNLCK;
                if( fcntl(fd,F_SETLKW, &f_lock) == -1)
                {
                        perror("File not unlocked\n");
                        exit(1);
                }
                printf("File unlocked\n");
		fclose( file );
	}
	bzero(buf,sizeof buf);


}

void updateLog( char * uname, int flag )
{
	char str_buf[BUFSIZ];
	bzero(str_buf, sizeof str_buf );
	const char filename[] = "logs.txt";
	FILE * file = fopen(filename,"a+");
        if(file == NULL)
        {
                printf("Cannot open users file.\n");
                exit(1);
        }
        struct flock f_lock;
	f_lock.l_type = F_WRLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
	if( flag == LOG_IN )
        {
		strcpy(str_buf, uname);
	        strcat(str_buf, " logged in :: ");
                strcat(str_buf,(char *) showTime() );
                strcat(str_buf, "\n");
                fputs( str_buf, file );

	}
	else if( flag == POST )
	{
                strcpy(str_buf, uname);
                strcat(str_buf, " posted successfully :: ");
                strcat(str_buf, showTime() );
		strcat(str_buf, "\n");
                fputs( str_buf, file );	

	}
	else if(flag == LOG_OUT )
	{
                strcpy(str_buf, uname);
                strcat(str_buf, " logged out :: ");
                strcat(str_buf, showTime() );
                strcat(str_buf, "\n");
                fputs( str_buf, file );

	}
	f_lock.l_type = F_UNLCK;
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not unlocked\n");
                exit(1);
        }
        printf("File unlocked\n");
	fclose(file);

}

void showDiscussion( int *client_socket )
{
        char buf1[BUFSIZ],buf[BUFSIZ];
	bzero(buf, sizeof buf);
	bzero(buf1, sizeof buf1);
        FILE * file = fopen(discno,"r");
        puts(discno);
	if(file == NULL) 
	{
		printf("Cannot open file.\n");
		send(*client_socket,"No discussion selected\n",23,0);
		return;
  	}
        struct flock f_lock;
	f_lock.l_type = F_RDLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
	while( fgets(buf,128,file) != NULL )
	        strcat(buf1,buf);
	puts(buf1);
	if(strcmp(buf1,"")==0)
		send(*client_socket,"No posts in this discussion\n",28,0);
	else
	        send(*client_socket,buf1,2000,0);
	f_lock.l_type = F_UNLCK;
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not unlocked\n");
                exit(1);
        }
        printf("File unlocked\n");
	fclose(file);
	bzero(buf,sizeof buf);
	bzero(buf1,sizeof buf1);
	
}
void updateWall(int * client_socket , char * uname)
{
	/*send signal to the user*/
}

void postComment( int * client_socket ,char * uname )
{
	char buf1[BUFSIZ],buf[BUFSIZ],disp_time[150];
	bzero(buf, sizeof buf);
        bzero(buf1, sizeof buf1);
        bzero(disp_time, sizeof disp_time);
	int nread;
        FILE * file = fopen(discno,"a+");
        if(file == NULL)
        {
                printf("Cannot open file.\n");
        	send(*client_socket,"No discussion selected\n",23,0);
		return;
        }
	send(*client_socket,"OK",2,0);
	nread = recv(*client_socket, buf,500, 0);
        struct flock f_lock;
	f_lock.l_type = F_WRLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = getFileSize( file );
        int fd = fileno( file );
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not locked\n");
                exit(1);
        }
        printf("File locked\n");
	fputs("###################################################################################################\n\n",file);
	fputs(buf,file);
	strcat(disp_time,"Posted by ");
	strcat(disp_time,uname);
	strcat(disp_time," on ");
	strcat(disp_time,showTime());
	fputs(disp_time,file);
	fputs("###################################################################################################\n",file);
	f_lock.l_type = F_UNLCK;
        if( fcntl(fd,F_SETLKW, &f_lock) == -1)
        {
                perror("File not unlocked\n");
                exit(1);
        }
	printf("File unlocked\n");
	bzero( buf, sizeof buf);
	strcpy(buf,"Posted Successfully\n\n");
	puts(buf);
	send( *client_socket,buf,25,0);
	updateLog( uname , POST );
	fclose(file);
	bzero(buf,sizeof buf);
	bzero(buf1,sizeof buf1);
	
}
