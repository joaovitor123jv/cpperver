#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <netdb.h> // for getnameinfo()

// Usual socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define SIZE 1024
#define BACKLOG 10 // Passed to listen()

class Request {
    private:
        int client_socket;
        const std::string http_header = "HTTP/1.1 200 OK\r\n\n";
        const std::string http_internal_error_header = "HTTP/1.1 500 ERROR\r\n\n";
        std::string response_data;

    public:
        Request(int client_socket) {
            this->client_socket = client_socket;
        }

        ~Request() {
            ::close(client_socket);
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
            std::string http_response = http_header + response_data;
            std::cout << "RESPONSE: " << http_response << std::endl;
            ::send(client_socket, http_response.c_str(), http_response.length(), 0);
        }

        void respond(std::string content) {
            std::string http_response = http_header + content;
            std::cout << "RESPONSE: " << http_response << std::endl;
            ::send(client_socket, http_response.c_str(), http_response.length(), 0);
        }

        void respond_server_error() {
            std::string http_response = http_internal_error_header + "INTERNAL ERROR";
            ::send(client_socket, http_response.c_str(), http_response.length(), 0);
        }
};