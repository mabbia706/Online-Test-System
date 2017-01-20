#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include<limits.h>
#include <time.h>
#include <arpa/inet.h>
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

int portnumber = 7891;
int client_no=0;

int isSubServer(char op[1024]){
	char str[]="sub_server";
	int i;
	for(i=0;i<strlen(str);i++){
		if(op[i]!=str[i]){
			return 0;
		}
	}
	return 1;
	
}
int isEqual(char op1[20],char op2[20]){
	int i;
	if(strlen(op1)!=strlen(op2)){
		
		return 0;
	}
	for(i=0;i<strlen(op2);i++){
		if(op1[i]!=op2[i]){
	
			return 0;
		}
	}
	return 1;
	
}
struct arg_struct {
    int socket;
	int port;
//socklen_t addr_size;
  // struct sockaddr_in clientAddr;
   //char message[1024];
};

void *ServerEcho(void *args)
{

        struct arg_struct *argms = args;
         int client_port=ntohs(argms->port);
        int client_socket=argms->socket;
         int nBytes;
        char buffer[1024];
        struct sockaddr_in  clientAddr;
        socklen_t addr_size = sizeof clientAddr;

        while(1){
        nBytes = recvfrom(client_socket,buffer,1024,0,(struct sockaddr *)&clientAddr, &addr_size);
           	
        sendto(client_socket,"Welcome",strlen("Welcome"),0,(struct sockaddr *)&clientAddr,addr_size);
	                client_no--;
		        close(client_socket);
		        pthread_exit(NULL);

	    break;

        }
	
}
//struct for storing test attributes
struct TestAttr {

   char  test_name[20];
   char  file_size[20];
   char  test_type[20];
   char  sub_server_ip[20];
   char  sub_server_port[20];
 
} TestAttr;
//fuction to check whether test is already stored in db or not
int alreadyInDb(struct TestAttr test)
{

struct TestAttr test_attr;
 /*** open the tests file ***/
   FILE *infile;
   infile = fopen ("Tests.dat","r");
   if (infile == NULL)
     {
      fprintf(stderr, "\nError opening Tests.dat\n\n");
      exit (1);
     }
 
 
   while (fread (&test_attr, sizeof(struct TestAttr), 1, infile)){
   if(strcmp(test_attr.test_name,test.test_name)==0 && strcmp(test_attr.file_size,test.file_size)==0 && strcmp(test_attr.test_type,test.test_type)==0 && strcmp(test_attr.sub_server_ip,test.sub_server_ip)==0 && strcmp(test_attr.sub_server_port,test.sub_server_port)==0)
   		return 1;
      /*printf ("Test Name = %s    File Size= %s   Test Type = %s	IP = %s	Port=%s\n",
          test_attr.test_name, test_attr.file_size, test_attr.test_type, test_attr.sub_server_ip, test_attr.sub_server_port);*/
   }
   return 0;

}
//searches for test in db and getting its attributes.
int searchTestInDb(char subject[20],char type[20],char IP[20],char port[20],char file_size[20])
{
	struct TestAttr test_attr;
	 /*** open the tests file ***/
   FILE *infile;
   infile = fopen ("Tests.dat","r");
   if (infile == NULL)
     {
      fprintf(stderr, "\nError opening Tests.dat\n\n");
      exit (1);
     }
while (fread (&test_attr, sizeof(struct TestAttr), 1, infile)){
	if(strcmp(test_attr.test_name,subject)==0 && isEqual(test_attr.test_type,type)==1 ){
			strcpy(IP,test_attr.sub_server_ip);
			strcpy(port,test_attr.sub_server_port);
			strcpy(file_size,test_attr.file_size);	
		   	return 1;
		}
      
   }
   return 0;
}

int main(){

  int udpSocket, nBytes,client_sock,c,newSocket;
	FILE *database;
		
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i;
	char operation[1024];
 	pthread_t t[5];	
char buffer[1024];
int ports[5];
  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof clientAddr;

  while(1){
//maximum 50 clients at a time can be connected
  if(client_no<50){
    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on clientAddr variable */
    	nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&clientAddr, &addr_size);
   	int port = ntohs(clientAddr.sin_port);
			
		   	printf("Connected to client %d\n",port);	
		   	printf("Client: %d sent message: %s\n",port,buffer);
			
			newSocket = socket(PF_INET, SOCK_DGRAM, 0);

			  /*Configure settings in address struct*/
			  serverAddr.sin_family = AF_INET;
			  serverAddr.sin_port = htons(portnumber);
			  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			  
			  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

			  /*Bind socket with address struct*/
			  bind(newSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

			  /*Initialize size variable to be used later on*/
			  //addr_size = sizeof clientAddr;	

			int is_sub_server=0;
			//tokenizing sent message from either subserver or client
			char* token = strtok(buffer, "/");
			while (token) {
			//checking if message is sent from client or subserver
					if(isSubServer(token)){
						database=fopen("Tests.dat","a");
						if(database!=NULL){
							printf("Writing into Database\n");
							char *end_str;
							//tokenizing on the base of / to separate test data
							char* temp_token = strtok_r(buffer, "/",&end_str);
							while(temp_token!=NULL){
								char *end_str1;

							//tokenizing on the base of : to separate multiple tests
        						char *tests = strtok_r(temp_token, ":", &end_str1);
								while(tests!=NULL){
									char *end_token;

							//tokenizing on the base of , to separate test attributes
        							char *test_values = strtok_r(tests, ",", &end_token);
									int count=0;
									struct TestAttr test_attr;
								
									while(test_values!=NULL){
										
										puts(test_values);
										if(count==0)
										strcpy(test_attr.test_name,test_values);
										else if(count==1)
										strcpy(test_attr.file_size,test_values);
										else if(count==2)
										strcpy(test_attr.test_type,test_values);
										else if(count==3)
										strcpy(test_attr.sub_server_ip,test_values);
										else
										strcpy(test_attr.sub_server_port,test_values);
										
										test_values = strtok_r(NULL, ",",&end_token);
										count++;	
									}
									if(alreadyInDb(test_attr)==0){
										//writing data into file
										fwrite (&test_attr, sizeof(struct TestAttr), 1, database);
									}
									else{
										printf("Already in Database\n");
									}
									tests = strtok_r(NULL, ":",&end_str1);
									
								}
								break;
							}
//replying to sub server
							sendto(newSocket,"Message received and saved successfully",strlen("Message received and saved successfully"),0,    								(struct sockaddr *)&clientAddr,addr_size);
							is_sub_server=1;
							//closing file
							fclose(database);
							close(newSocket);
							
							printf("Done\n");
							break;
					     }
						
					}
				  token = strtok(NULL, "/"); 				    
			}
			//if message is sent by client
			if(is_sub_server==0){

				char curr_port[5];
				snprintf(curr_port, 5,"%d",portnumber);
				struct arg_struct args;
	    			args.socket = newSocket;
				args.port=portnumber;
				/*args.addr_size=addr_size;
				memset(	args.message, 0, sizeof(	args.message));
				for(int i=0;buffer[i]!='\0';i++){
				args.message[i]=buffer[i];
				}*/
				//declaring test attributes that will be sent to client
				char requested_subject[20];
				char requested_type[20];
				char subserver_ip[20];
				char subserver_port[20];
				//search for sub server containing required test
				char* token = strtok(buffer, " ");
				int word_no=0;
				char reply_client[100];
				char file_size[20];
				//char sub_servers_list[][1];

				while (token) {
					
					if(word_no==0){
						//first token is for subject
						strcpy(requested_subject,token);
					}
					else if (word_no==1){
						//second is for type
						strcpy(requested_type,token);
					}
					token = strtok(NULL, " ");
					word_no++;
				}
			//if test is found in file
				if(searchTestInDb(requested_subject,requested_type,subserver_ip,subserver_port,file_size)==1){
					//clearing buffer for every request						
					memset(reply_client, 0, 100);
					//concatening to send test and subserver addr to cleint
					strcat(reply_client,subserver_ip);
					strcat(reply_client," ");
					strcat(reply_client,subserver_port);

					strcat(reply_client," ");
					strcat(reply_client,file_size);

				}
				else{
					memset(reply_client, 0, 100);
					strcat(reply_client,"404");
				}
				//replying to cleint
				sendto(newSocket,reply_client,strlen(reply_client),0,(struct sockaddr *)&clientAddr,addr_size);
				//assigining handler to cleint for further communication				
				pthread_create(&t[client_no],NULL,ServerEcho,(void *)&args);
				portnumber++;
				client_no++;
			}
		}
	
	
 }			
  	
 

  return 0;
}
