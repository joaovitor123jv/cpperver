#pragma once

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <netdb.h> // for getnameinfo()

// Usual socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include "request.hpp"

#define SIZE 1024
#define BACKLOG 10 // Passed to listen()

class Cerver {
    private:
        unsigned int port;
        int socket;
        struct sockaddr_in *address;
        
        bool (*requests_handler)(Request *request);

        int create_socket(void) {
            // Socket setup: creates an endpoint for communication, returns a descriptor
            // -----------------------------------------------------------------------------------------------------------------
            return ::socket( // Global scope, not the current class one
                AF_INET,     // Domain: specifies protocol family
                SOCK_STREAM, // Type: specifies communication semantics
                0            // Protocol: 0 because there is a single protocol for the specified family
            );
        }

        struct sockaddr_in *create_address(unsigned int port) {
            // Construct local address structure
            // -----------------------------------------------------------------------------------------------------------------
            struct sockaddr_in *server_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
            server_address->sin_family = AF_INET;
            server_address->sin_port = htons(port);
            server_address->sin_addr.s_addr = htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");

            return server_address;
        }

        void show_status() {
            char *host_buffer = (char *)calloc(INET6_ADDRSTRLEN, sizeof(char));
            char *service_buffer = (char *)calloc(NI_MAXSERV, sizeof(char)); // defined in `<netdb.h>`

            int err = getnameinfo(
                (struct sockaddr *)this->address,
                (socklen_t)sizeof(struct sockaddr_in),
                host_buffer,
                INET6_ADDRSTRLEN,
                service_buffer,
                NI_MAXSERV,
                NI_NUMERICHOST
            );

            if (err != 0) std::cout << "It's not working!!\n" << std::endl;

            std::cout << "Server listening on http://" << std::string(host_buffer) << ":" << std::string(service_buffer) << std::endl;
            free(host_buffer);
            free(service_buffer);
            host_buffer = NULL;
            service_buffer = NULL;
        }
    
        Request listen() {
            int client_socket = ::accept(this->socket, NULL, NULL);
            std::cout << "Request detected, processing" << std::endl;

            return Request(client_socket);
        }

    public:
        Cerver(unsigned int port) {
            this->socket = create_socket();
            this->address = create_address(port);

            // Bind socket to local address
            // -----------------------------------------------------------------------------------------------------------------
            // bind() assigns the address specified by serverAddress to the socket
            // referred to by the file descriptor serverSocket.
            bind(this->socket, (struct sockaddr *)this->address, sizeof(struct sockaddr_in));

            if (::listen(this->socket, BACKLOG) < 0) {
                printf("Error: The server is not listening.\n");
                exit(EXIT_FAILURE);
            }

            show_status();
        }

        ~Cerver() {
            ::close(this->socket);
            free(address);
            address = NULL;
        }

        void process_requests_with(bool (*handler_function)(Request *request)) {
            this->requests_handler = handler_function;
        }

        void start() {
            while(true) {
                Request request = this->listen();

                std::cout << "LALA" << std::endl;

                bool everything_is_fine = (this->requests_handler)(&request);

                if(everything_is_fine) {
                    std::cout << "ALL OK" << std::endl;
                    request.respond();
                } else {
                    std::cout << "ALL NOT OK" << std::endl;
                    request.respond_server_error();
                }


                // request.append_file("index.html");
                // request.respond();
            }
        }

};