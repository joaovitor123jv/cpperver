#pragma once

#ifndef __CERVER_HEADER
#define __CERVER_HEADER

#include <iostream>
#include <errno.h>

#if _WIN32
    // windows code goes here
    #include<winsock2.h>
#else
    #include <netdb.h> // for getnameinfo()
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

// Usual socket headers
#include <sys/types.h>

#include "request.hpp"
#include "http_request.hpp"
#include "http_router.hpp"
#include "time_utilities.hpp"

#define SIZE 1024
#define BACKLOG 10 // Passed to listen()

class Cerver {
    private:
        #if _WIN32
            SOCKET socket;
        #else // Linux/macOS/*BSD
            int socket;
        #endif

        unsigned int port;
        
        struct sockaddr_in *address;
        
        bool (*requests_handler)(Request *request);

        #if _WIN32
            SOCKET create_socket(void) {
                SOCKET created_socket = ::socket(AF_INET, SOCK_STREAM, 0);

                //Create a socket
                if(created_socket == INVALID_SOCKET) std::cout << "Could not create socket: " << WSAGetLastError() << std::endl;

                std::cout << "Socket created." << std::endl;
                return created_socket;
            }

        #else
            int create_socket(void) {
                // Socket setup: creates an endpoint for communication, returns a descriptor
                // -----------------------------------------------------------------------------------------------------------------
                return ::socket( // Global scope, not the current class one
                    AF_INET,     // Domain: specifies protocol family
                    SOCK_STREAM, // Type: specifies communication semantics
                    0            // Protocol: 0 because there is a single protocol for the specified family
                );
            }
        #endif
        

        struct sockaddr_in *create_address(unsigned int port) {
            // Construct local address structure
            // -----------------------------------------------------------------------------------------------------------------
            struct sockaddr_in *server_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
            server_address->sin_family = AF_INET;
            server_address->sin_port = htons(port);

            #if _WIN32
                server_address->sin_addr.s_addr = INADDR_ANY;
            #else
                server_address->sin_addr.s_addr = htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");
            #endif

            return server_address;
        }

        void show_status() {
            #if _WIN32
                std::cout << "Cannot show status currently on WIN32 platforms" << std::endl;
            #else
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
            #endif // _WIN32
        }
    
        int listen() {
            std::cout << "Waiting for incoming connections..." << std::endl;

            #if _WIN32
                int client_length = (int)sizeof(struct sockaddr_in);
                struct sockaddr_in client;
                SOCKET client_socket = ::accept(this->socket , (struct sockaddr *)&client, &client_length);
                if (client_socket == INVALID_SOCKET)
                    std::cout << "Failed to accept request with error code : " << WSAGetLastError() << std::endl;
            #else
                int client_socket = ::accept(this->socket, NULL, NULL);
            #endif

            std::cout << "Request detected, processing" << std::endl;

            return (int)client_socket;
        }

        #if _WIN32
            bool initialize_winsocket() {
                std::cout << "Initialising Winsock..." << std::endl;
                if (WSAStartup(MAKEWORD(2, 2), &(this->wsa_data)) != 0)
                {
                    std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
                    return false;
                }
                
                std::cout << "Initialised." << std::endl;
                return true;
            }
        #endif

    public:
        #if _WIN32
            static WSAData wsa_data; // Windows socket initialization
            static bool initialized;
        #endif

        Cerver(unsigned int port) {
            #if _WIN32
                if(this->initialized == false) this->initialize_winsocket();
            #endif

            this->socket = create_socket();
            this->address = create_address(port);
            
            #if _WIN32
                //Bind
                int bind_result = bind(this->socket, (struct sockaddr *)this->address, sizeof(struct sockaddr_in));
                if(bind_result == SOCKET_ERROR) std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
            #else // Linux/BSD/macOS
                bind(this->socket, (struct sockaddr *)this->address, sizeof(struct sockaddr_in));
            #endif


            if (::listen(this->socket, BACKLOG) < 0) {
                printf("Error: The server is not listening.\n");
                exit(EXIT_FAILURE);
            }

            show_status();
        }

        ~Cerver() {
            this->close_connection();
            free(address);
            address = NULL;
        }

        void close_connection() {
            #if _WIN32
                closesocket(this->socket);
                WSACleanup();
            #else // macOS, Linux, *BSD
                ::close(this->socket);
            #endif
        }

        void process_requests_with(bool (*handler_function)(Request *request)) {
            this->requests_handler = handler_function;
        }

        void start() {
            Stopwatch request_timer = Stopwatch();

            while(true) {
                Request request = Request(this->listen());
                request_timer.start();

                bool executed_successfully = (this->requests_handler)(&request);

                if(executed_successfully) {
                    std::cout << "ALL OK" << std::endl;
                    request.respond();
                }
                request_timer.finish();
                request_timer.print_elapsed_time();

                request_timer.reset_times();
            }
        }

        void start(HttpRouter *router) {
            if(router == NULL) {
                std::cout << "Failed to start server. Router is null" << std::endl;
            }
            Stopwatch request_timer = Stopwatch();

            while(true) {
                HttpRequest request = HttpRequest(this->listen());
                request_timer.start();

                request.receive_client_data();

                std::string uri = "blablaa";
                std::string http_method = "GET";

                HttpRequestHandler handler = router->get_handler(uri, http_method);

                bool executed_successfully = handler(&request);

                if(executed_successfully) {
                    std::cout << "ALL OK" << std::endl;
                    request.respond();
                } else {
                    std::cout << "ALL NOT OK" << std::endl;
                    // TODO: Send response as HttpRequest
                    // request.respond_server_error();
                }
                request_timer.finish();
                request_timer.print_elapsed_time();
                request_timer.print_elapsed_time();
            }
        }
};

#if _WIN32
    WSAData Cerver::wsa_data = {};
    bool Cerver::initialized = false;
#endif

#endif // __CERVER_HEADER