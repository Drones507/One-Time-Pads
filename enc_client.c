/***************************************************************
 * Filename: enc_client.c
 * Author: Christopher Harvey
 * Date: June 3 2024
 * Description: Main file to get the plaintext & key, CS 374
***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

#define MAX_SIZE 7000
#define BUFFER_SIZE 7000

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

//read argv[]'s
void read_file(char *filename, char *buffer, size_t buffer_size){
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(1);
    }
    if (!fgets(buffer, buffer_size, file)) {
        fprintf(stderr, "Error: cannot read from file %s\n", filename);
        fclose(file);
        exit(1);
    }
    fclose(file);

    // Remove newline character if present
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

//Validate characters in plaintext & key, also validate plaintext & key have equal lengths
void validate_input(const char *plaintext, const char *key) {
    size_t plaintext_len = strlen(plaintext);
    size_t key_len = strlen(key);

    if (key_len < plaintext_len) {
        fprintf(stderr, "Error: key is shorter than plaintext\n");
        exit(1);
    }
    
    for (size_t i = 0; i < plaintext_len; i++) {
        if ((plaintext[i] < 'A' || plaintext[i] > 'Z') && plaintext[i] != ' ') {
            fprintf(stderr, "Error: invalid character in plaintext\n");
            exit(1);
        }
    }

    for (size_t i = 0; i < key_len; i++) {
        if ((key[i] < 'A' || key[i] > 'Z') && key[i] != ' ') {
            fprintf(stderr, "Error: invalid character in key\n");
            exit(1);
        }
    }
}

int main(int argc, char* argv[]){
     int socketFD, portNumber, charsWritten, charsRead;
     struct sockaddr_in serverAddress;
     char buffer[BUFFER_SIZE];
     char plaintext[BUFFER_SIZE];
     char key[BUFFER_SIZE];

     //Check usage & args
    if (argc < 3) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
    exit(0); 
    } 

    //Get plaintext
    read_file(argv[3], plaintext, BUFFER_SIZE);
    //Get key
    read_file(argv[4], key, BUFFER_SIZE);
    //Validate 
    validate_input(plaintext, key);

    //Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFD < 0){
        error("CLIENT: ERROR opening socket");
    }
    
    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[2]), argv[1]);

    //Connect the socket to server with connect()
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        error("CLIENT: ERROR connecting");
    }

/*INTEREST:: SETUP TO SEND PLAINTEXT & KEY TO SERVER*/
    // Get input message from user
    printf("CLIENT: Sending %s and %s...\n", plaintext, key);
    // Clear out the buffer array
    // memset(buffer, '\0', sizeof(buffer));
    // Get input from the user, trunc to buffer - 1 chars, leaving \0
    // fgets(buffer, sizeof(buffer) - 1, stdin);
    // Remove the trailing \n that fgets adds
    // buffer[strcspn(buffer, "\n")] = '\0'; 

    // Send plaintext message to server
    // Write to the server
    charsWritten = send(socketFD, plaintext, strlen(plaintext), 0); 
    if (charsWritten < 0){
        error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(plaintext)){
        printf("CLIENT: PLAINTEXT ~ WARNING: Not all data written to socket!\n");
    }
    //Send key to server
    //Write to the server
    charsWritten = send(socketFD, key, strlen(plaintext), 0); 
    if (charsWritten < 0){
        error("CLIENT: ERROR writing to socket");
    }
    // if (charsWritten < strlen(key)){
    //     printf("CLIENT: KEY ~ WARNING: Not all data written to socket!\n");
    // }
    // Get return message from server
    // Clear out the buffer again for reuse
    memset(plaintext, '\0', sizeof(plaintext));
    memset(key, '\0', sizeof(key));
    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        error("CLIENT: ERROR reading from socket");
    }
    printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

    // Close the socket
    close(socketFD); 
    return 0;
}