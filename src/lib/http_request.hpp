#ifndef __CERVER_HTTP_REQUEST
#define __CERVER_HTTP_REQUEST

#if _WIN32
    #include <WinSock2.h>
#endif

#include "request.hpp"

#define __CERVER_HTTP_REQUEST_BUFFERSIZE 1024

class HttpRequest : public Request {
    private:
        const static std::string http_header;
        const static std::string http_internal_error_header;
        char receive_buffer[__CERVER_HTTP_REQUEST_BUFFERSIZE];

        std::string received_request;

        void reset_receive_buffer() {
            for(int i=0; i < __CERVER_HTTP_REQUEST_BUFFERSIZE; i++) this->receive_buffer[i] = '\0';
        }

    public:
        HttpRequest(int client_socket) : Request{ client_socket } {}

        bool receive_client_data() {
            int received_bytes_number = 0;
            unsigned int total_bytes_received_number = 0;
            this->reset_receive_buffer();

            do {
                received_bytes_number = recv(
                    this->client_socket,
                    this->receive_buffer,
                    __CERVER_HTTP_REQUEST_BUFFERSIZE,
                    0 // Flags
                );

                if (received_bytes_number > 0) {

                    std::cout << "Bytes received: " << received_bytes_number << std::endl;
                    total_bytes_received_number += received_bytes_number;

                    this->received_request.append(this->receive_buffer);
                    this->reset_receive_buffer();
                }
                else if (received_bytes_number == 0) {
                    std::cout << "Finished receiving bytes from client..." << std::endl;
                }
                else {
                    std::cout << "Failed to receive data: " << WSAGetLastError() << std::endl;
                    this->close_connection();
                    return false;
                }

            } while (received_bytes_number == __CERVER_HTTP_REQUEST_BUFFERSIZE);


            std::cout << "Final received request: " << std::endl << this->received_request << std::endl;
            return true;
        }
    
        bool process_received_request() {
            // TODO: implement (probably an automata to read all lines and extract info about HTTP method, URL, Host...)s
        }
};

const std::string HttpRequest::http_header = "HTTP/1.1 200 OK\r\n\n";
const std::string HttpRequest::http_internal_error_header = "HTTP/1.1 500 ERROR\r\n\n";

#endif // __CERVER_HTTP_REQUEST