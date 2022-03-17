#include <stdio.h>
#include <string.h>     // for strlen
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <pthread.h>   // for threading, link with lpthread
#include <time.h>      // for getting  time

// tm struct for time operations
struct Tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

int main(int argc, char *argv[])
{

    int socket_desc, new_socket, c, *new_sock;  // sockets
    struct sockaddr_in server, client;
    char *message;                  // this is used for sending message
    char *received_message[1000];   // this is used for receiving message

    struct Tm *info;                // for formatting date
    time_t current_time;            // taking current time
    char *c_time_string[100];       // formatting current time

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);  //creating socket
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);  // defining port

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)  // bining socket
    {
        puts("Binding failed");
        return 1;
    }

    listen(socket_desc, 3);                         // listen operation for socket
    puts("Waiting for incoming connections...");    // server side message.
    c = sizeof(struct sockaddr_in);             

    while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Connection accepted");                // successful connection accepting server side message.

        // MESSAGE FOR CLIENT
        message = "\nW E L C O M E\n\nVALID PARAMETERS= \%a, \%A, \%b, \%B, \%c, \%d, \%H, \%I, \%j, \%m, \%M, \%p, \%S, \%U, \%w, \%W, \%x, \%X, \%y, \%Y, \%Z\n";
        write(new_socket, message, strlen(message));

        // INFINITE LOOP FOR CLIENT
        while (1)
        {
            // GETTING FORMAT
            message = "\n> GET_DATE ";
            write(new_socket, message, strlen(message));
            recv(new_socket, received_message, 1000, 0);

            // DISPLAYING RECEIVED FORMAT ON SERVER SIDE
            puts("REQUEST RECEIVED AS: ");
            puts(received_message);

            // valid characters are aAbBcdHIjmMpSUwWxXyYZ
            const char *invalid_characters = "CDeEfFgGhiJkKlLnNoOPrRsuvVtTz";
            char *c = received_message;

            // for checking variable
            int incorrect_request = 0;

            while (*c)
            {
                if (strchr(invalid_characters, *c))
                {
                    incorrect_request = 1;
                    break;
                }
                c++;
            }
            
            if (incorrect_request == 1) // FOR INCORRECT REQUEST
            {
                message = "\n> INCORRECT REQUEST\n";
                write(new_socket, message, strlen(message));
            }
            else                        // FOR CORRECT REQUEST
            {
                time(&current_time);                                    // GETTING CURRENT TIME
                info = localtime(&current_time);                        // ASSIGN TO A VARIABLE
                strftime(c_time_string, 100, received_message, info);   // CALLING strftime for formatting DATE
                
                // sending formatted message to the client server
                message = c_time_string;                                
                write(new_socket, message, strlen(message));
            }
        }

        // CLOSING SOCKETS
        close(socket_desc);
        close(new_socket);
    }

    return 0;
}
