#ifndef __CERVER_REQUEST_HEADER
#define __CERVER_REQUEST_HEADER

#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>

#if _WIN32
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



#define SIZE 1024


class Request {
    protected:
        int client_socket;
        std::string response_data;

    public:
        Request(int client_socket) {
            this->client_socket = client_socket;
        }

        ~Request() {
            this->close_connection();
        }
        
        void close_connection() {
            #if _WIN32
                ::closesocket(client_socket);
            #else // macOS, Linux, *BSD
                ::close(this->socket);
            #endif
        }

        void append_file(std::string file_name) {
            std::ifstream file;
            file.open(file_name);

            if (file.is_open()) {
                std::string line;

                while(getline(file, line)) response_data.append(line);
            }
        }

        void respond() {
            std::cout << "RESPONSE: " << response_data << std::endl;
            ::send(client_socket, response_data.c_str(), response_data.length(), 0);
        }

        void respond(std::string content) {
            std::cout << "RESPONSE: " << content << std::endl;
            ::send(client_socket, content.c_str(), content.length(), 0);
        }
};


#endif // __CERVER_REQUEST_HEADER