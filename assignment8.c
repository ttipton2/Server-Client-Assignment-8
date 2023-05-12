/*Name: Tim Tipton
 * Class: CS360
 * Instructor: Ben McCamish
 * Assignment: HW8
 * Date: 11/8/2022
 * Abstract: This is a program that either sets up a server or a client depending on the command line arguments provided.
 */

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

#define CLIENT_PORT 49999
#define MAX 100
#define LISTENBUF 10

extern int errno;

void hostnameChk(int hostname) {
  if (hostname == -1) {
    perror("Error");
    exit(-1);
  }
}

//function to change the hostname to an ip address, takes two pointers two a string, one for the hostname and one for the ip address
int hostnameToIpAddr(char * hostname, char * ip) {
  struct hostent *host;
  struct in_addr **addressList;
  int i;
  if ((host = gethostbyname(hostname)) == NULL) {
    return -1;
  }
  addressList = (struct in_addr **) host->h_addr_list;

  for (i=0; addressList[i] != NULL; i++) {
    strcpy(ip, inet_ntoa(*addressList[i]));
    return 0;
  }
  return -1;
}
//function to make sure that the host entry isn't null, takes a hostent as the argument
void hostEntryChk(struct hostent * hostEntry) {
  if (hostEntry == NULL) {
    perror("Error");
    exit(-1);
  }
}

int main(int argc, char **argv) {
    int clientID = 0;
    char error[] = "Unknown name or service.\n";
    //this is the logic for the client connection. It checks to see if the user chose client on the command line and performs the necessary operations per the writeup.
    if (strcmp(argv[1], "client") == 0){//checks to see if the user is trying to use this program as a client
      if(argc != 3){
        printf("Error: No hostname or IP address specified on the command line\n");
        exit(-1);
      }
      int socketFd;

      if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //logic to make a client socket, throws an error if unsuccessful.
        perror("Error");
        exit(-1);
      }

      char serverResponse[20];
      struct sockaddr_in serverAddress;

      char *hostname = argv[2];
      char ipAddress[100];
      hostnameToIpAddr(hostname, ipAddress);

      serverAddress.sin_family = AF_INET;
      serverAddress.sin_addr.s_addr = inet_addr(ipAddress);
      serverAddress.sin_port = htons(CLIENT_PORT);

      int err = connect(socketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));//logic for the client to connect to the server.
      if (err != 0) {
        printf("Error: %s\n", error);//prints the error to stdout in the appropriate format.
        exit(-1);
      }

      if (recv(socketFd, serverResponse, 19, 0) < 0) {
        perror("Error");
        exit(-1);
      }
      else{
        serverResponse[strlen(serverResponse)-1] = '\0';
        printf("%s\n", serverResponse);
        return 0;
      }
    }


    if (strcmp(argv[1], "server") == 0) {//logic to check and see if server was specified on the command line and returns an error if not
      if(argc != 2){
        exit(-1);
      }
      int listenFd;

      if (((listenFd) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error");
        exit(-1);
      } else {
      struct sockaddr_in serverAddress;
      memset(&serverAddress, '\0', sizeof(serverAddress));

      serverAddress.sin_family = AF_INET;
      serverAddress.sin_port = htons(CLIENT_PORT);
      serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

      int f = bind( listenFd,(struct sockaddr *) &serverAddress, sizeof(serverAddress));

        if (f != 0) {
            perror("Error"); // If the server is already open, print error and exit.
            exit(-1);
        }
      }

      if (listen(listenFd, 1) < 0) { //logic to listen for and set up a connection, exits with error if unsucessful.
        perror("Error");
        exit(-1);
      }

      while(1){
        int clientSocket = accept(listenFd, NULL, NULL);
        if (clientSocket < 0) {
          perror("Error");
          exit(-1);
        }
        clientID++;
          while(1) {
            time_t clientTime;
            time(&clientTime);
            send(clientSocket, ctime(&clientTime), 30, 0);
            if (send(clientSocket, ctime(&clientTime), 30, 0) < 0) {
              perror("Error");
              exit(-1);
            }
            char hostBuffer[256];
            int hostname = gethostname(hostBuffer, sizeof(hostBuffer));

            if (hostname < 0) {
              perror("Error");
              exit(-1);
            }
            hostnameChk(hostname);

            printf("%s %d\n", hostBuffer, clientID);
            break;

          }
        }
        close(listenFd);
      return 0;
    }

    else {//logic to check edge case where no command line args were provided
      printf("Error: No command line argument provided.");
      exit(-1);
    }
return 0;
}
