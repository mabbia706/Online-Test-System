#include<stdio.h>
#include<limits.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // open function
#include <unistd.h> // close function

#define SPACE "\n"
//below two functions are used for removing unwanted characters from string
static void strip(char *s)
{
    char *p = s;
    int n;
    while (*s)
    {
        n = strcspn(s, SPACE);
        strncpy(p, s, n);
        p += n;
        s += n + strspn(s+n, SPACE);
    }
    *p = 0;
}

static char *strip_copy(char const *s)
{
    char *buf = malloc(1 + strlen(s));
    if (buf)
    {
        char *p = buf;
        char const *q;
        int n;
        for (q = s; *q; q += n + strspn(q+n, SPACE))
        {
            n = strcspn(q, SPACE);
            strncpy(p, q, n);
            p += n;
        }
        *p++ = '\0';
        buf = realloc(buf, p - buf);
    }
    return buf;
}
static void strip(char *s);
static char *strip_copy(char const *s);
int ssize_t2int(ssize_t val) {
    return (val <= INT_MAX) ? (int)((ssize_t)val) : -1;
}
//the thread function
void *connection_handler(void *);
//struct for storing answering attributes
struct AnsAttr {

   char  answer[50];
   char  subject[50];
   char  client_id[50];
 
} AnsAttr; 

//struct for storing test attributes
struct Test {
   char  subject[50];
   char  type[50];
   char  size[50];
 
} Test;

//prints the data stored answers
void printAnsDb()
{
	struct AnsAttr ans_attr;
   FILE *infile;
   infile = fopen ("Answers.dat","r");
   if (infile == NULL)
     {
      fprintf(stderr, "\nError opening Answers.dat\n\n");
      exit (1);
     }
 
 	
   while (fread (&ans_attr, sizeof(struct AnsAttr), 1, infile)){
      printf ("Answer = %s    Subject = %s   Client ID = %s\n",
          ans_attr.answer, ans_attr.subject, ans_attr.client_id);
   }

}
  //file size is size of size of file goint to be sent
int file_size = 0;
//bin data will contain data of file in bytes
char* bin_data;
	
int main(int argc , char *argv[])
{
	const int total_tests=2;
    struct Test test[total_tests];
	struct stat file_stat;
	
	
	//open file
	FILE* fp = fopen("pic.jpg","rb+");
	//get its size
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	rewind (fp);

	//malloc the buffer big enough for image data

	bin_data = (char*) malloc (file_size);
	//read image data into buffer
	fread (bin_data,1,file_size,fp);	
	fclose(fp);
	char size[20];
	sprintf(size, "%d", file_size);

	// 1st test record
     strcpy(test[0].subject, "Mathematics");
     strcpy(test[0].type, "Image");
     strcpy(test[0].size, size);
     // 2nd test record         
     strcpy(test[1].subject, "Physics");
     strcpy(test[1].type, "Image");
     strcpy(test[1].size, size);

    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     int port=7777;
    //Prepare the sockaddr_in structure to wait for connections of client
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );

    int clientSocket, portNum, nBytes;
  char buffer[1024];
//creating udp socket in order to communicate with main server
  struct sockaddr_in mainServerAddr;
  socklen_t addr_size;

  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  char IP[50];
	strcpy(IP,"127.0.0.1");
  /*Configure settings in address struct*/
  mainServerAddr.sin_family = AF_INET;
  mainServerAddr.sin_port = htons(7891);
  mainServerAddr.sin_addr.s_addr = inet_addr(IP);
  memset(mainServerAddr.sin_zero, '\0', sizeof mainServerAddr.sin_zero);  

  /*Initialize size variable to be used later on*/
  addr_size = sizeof mainServerAddr;
//communicate with the main server
	   
char message[1000];
int i=0;
//concatenating information of this sub server in order to send this information to main server
for(;i<total_tests;i++){

	if(i>0){
		strcat(message,":");
	}
	strcat(message,test[i].subject);	
	strcat(message,",");	
	strcat(message,test[i].size);
	strcat(message,",");	
	strcat(message,test[i].type);
	char curr_port[10];
	snprintf(curr_port, 5,"%d",port);
	strcat(message,",");	
	strcat(message,IP);
	strcat(message,",");
	strcat(message,curr_port);
}	
	strcat(message,"/");
	strcat(message,"sub_server");
/*Send message to server*/
	    sendto(clientSocket,message,strlen(message),0,(struct sockaddr *)&mainServerAddr,addr_size);

	    /*Receive message from server*/
		        nBytes = recvfrom(clientSocket,buffer,1024,0,NULL, NULL);
			int k;
			//allocating received bytes to new array so that. Bytes that are left in buffer are not considered
			char receivedMessage[nBytes];
			for(k=0;k<nBytes;k++){
				receivedMessage[k]=buffer[k];
			}
	    printf("Received from server: %s\n",receivedMessage);
    	close(clientSocket);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 10);
     
     
    //Accept and incoming connection
//waits for client to make tcp connection
    puts("Sub Server 1 is waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         //assigning thread to each connected client
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
    int waiting_for_vote=0; 
    int word_no=0;
    size_t toread = read_size;
    while( (read_size = recv(sock , client_message , sizeof(client_message) , 0)) > 0 )
    {
	//separating immediately sent message from client
	int len=ssize_t2int(read_size);
	int q=0;

	char final_message[len];
	for(;q<len;q++){
		final_message[q]=client_message[q];		
	}
	final_message[q]='\0';
	char*stripped=strip_copy(final_message);
	//tokenizing message sent from client
	char* token = strtok(stripped, ":");
	//checking whether answer is being sent or request for data
	if(strcmp(token,"Answer")==0){
		
		struct AnsAttr ans_attr;
		FILE *ansData=fopen("Answers.dat","a");
		while (token) {
					
			if(word_no==0){

			}
			else if (word_no==1){
				strcpy(ans_attr.answer,token);
				puts("Answer given by client");
				puts(token);
			}else if (word_no==2){
				strcpy(ans_attr.subject,token);
				puts("Subject of the test");
				puts(token);
			}else if (word_no==3){
				strcpy(ans_attr.client_id,token);
				puts("ID of client");
				puts(token);
			}

				token = strtok(NULL, ":");
				word_no++;
		}
		fwrite (&ans_attr, sizeof(struct AnsAttr), 1, ansData);
		fclose(ansData);
puts("Printing answers data:\n");		
		printAnsDb();
				//706 code for approving client that message is stored in db
write(sock,"706",strlen("706"));
		
	}
	else{
		//Up to this point we are reading file content into a string. Now send this data across the network
		write(sock,bin_data,file_size);
	}
	
	//Send the message back to client

	//write(sock , client_message , strlen(client_message));
	

    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}
