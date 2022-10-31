// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";

    // Display ASLR
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    //--------Separating Process here------
    int status;
    struct passwd *pwd;
    char *name = "nobody";
    pid_t childProcessID = fork();
    pid_t waitPID;
    int id;
    
    if(childProcessID == 0){
        printf("The ID of chld process before dropping privileges is: %d \n", getuid());
        pwd = getpwnam(name);
        
        if (pwd ==  NULL){
            printf("Unable to find UID for the user %s\n", name);
            return 0;
        }
        
        // changing UID to nobody
        else{
	    setuid(pwd->pw_uid);
            printf("The ID of chld process after dropping privileges is: %d \n", getuid());
        }
        
        valread = read( new_socket , buffer, 1024);
        printf("\n%d bytes read in the message: %s\n", valread, buffer);
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
        return 0;
    }
    
    else{
        status = 0;
        while((waitPID = wait(&status)) > 0);
        printf("Exiting child process, going back to parent\n");
    }
}
