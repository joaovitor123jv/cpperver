#ifndef __CERVER_HEADER
#define __CERVER_HEADER

#include <iostream>
#include <errno.h>
#include <sys/types.h>

#if _WIN32
    #include <winsock2.h>
#else // _WIN32
    #include <netdb.h> // for getnameinfo()
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif // else _WIN32

#include "request.hpp"
#include "http_request.hpp"
#include "http_router.hpp"
#include "time_utilities.hpp"

#define SIZE 1024
#define BACKLOG 10 // Passed to listen()

class Cerver
{
private:
    unsigned int port;
    struct sockaddr_in *address;
    bool (*requests_handler)(Request *request);

#if _WIN32
    SOCKET socket;
#else // _WIN32
    int socket;
#endif // else _WIN32

#if _WIN32
    /**
     * @brief Create a socket object
     * 
     * @return SOCKET 
     */
    SOCKET create_socket(void)
    {
        SOCKET created_socket = ::socket(AF_INET, SOCK_STREAM, 0);

        //Create a socket
        if (created_socket == INVALID_SOCKET)
            std::cout << "Could not create socket: " << WSAGetLastError() << std::endl;

        std::cout << "Socket created." << std::endl;
        return created_socket;
    }

#else // _WIN32
    /**
     * @brief Create a socket object
     * 
     * @return int 
     */
    int create_socket(void)
    {
        // Socket setup: creates an endpoint for communication, returns a descriptor
        // -----------------------------------------------------------------------------------------------------------------
        return ::socket( // Global scope, not the current class one
            AF_INET,     // Domain: specifies protocol family
            SOCK_STREAM, // Type: specifies communication semantics
            0            // Protocol: 0 because there is a single protocol for the specified family
        );
    }
#endif // else _WIN32

    /**
     * @brief Create an address object
     * 
     * @return struct sockaddr_in* 
     */
    struct sockaddr_in *create_address()
    {
        // Construct local address structure
        // -----------------------------------------------------------------------------------------------------------------
        struct sockaddr_in *server_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        server_address->sin_family = AF_INET;
        server_address->sin_port = htons(port);

#if _WIN32
        server_address->sin_addr.s_addr = INADDR_ANY;
#else // _WIN32
        server_address->sin_addr.s_addr = htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");
#endif // else _WIN32

        return server_address;
    }

    /**
     * @brief Show the created server status
     * 
     */
    void show_status()
    {
#if _WIN32
        std::cout << "Cannot show status currently on WIN32 platforms" << std::endl;
#else // _WIN32
        char *host_buffer = (char *)calloc(INET6_ADDRSTRLEN, sizeof(char));
        char *service_buffer = (char *)calloc(NI_MAXSERV, sizeof(char)); // defined in `<netdb.h>`

        int err = getnameinfo(
            (struct sockaddr *)this->address,
            (socklen_t)sizeof(struct sockaddr_in),
            host_buffer,
            INET6_ADDRSTRLEN,
            service_buffer,
            NI_MAXSERV,
            NI_NUMERICHOST);

        if (err != 0)
        {
            std::cout << "It's not working!!" << std::endl;
        }

        std::cout << "Server listening on http://"
                  << std::string(host_buffer) << ":"
                  << std::string(service_buffer) << std::endl;
        free(host_buffer);
        free(service_buffer);
        host_buffer = NULL;
        service_buffer = NULL;
#endif // else _WIN32
    }

    /**
     * @brief Start listening for connections on server. Returns a client socket when connected
     * 
     * @return int 
     */
    int listen()
    {
        std::cout << "Waiting for incoming connections..." << std::endl;

#if _WIN32
        int client_length = (int)sizeof(struct sockaddr_in);
        struct sockaddr_in client;
        SOCKET client_socket = ::accept(this->socket, (struct sockaddr *)&client, &client_length);
        if (client_socket == INVALID_SOCKET)
            std::cout << "Failed to accept request with error code : " << WSAGetLastError() << std::endl;
#else // _WIN32
        int client_socket = ::accept(this->socket, NULL, NULL);
#endif // else _WIN32

        std::cout << "Request detected, processing" << std::endl;

        return (int)client_socket;
    }

#if _WIN32
    /**
     * @brief Initializes winsocket (WSA) to allow sockets creation
     * 
     * @return true 
     * @return false 
     */
    bool initialize_winsocket()
    {
        std::cout << "Initialising Winsock..." << std::endl;
        if (WSAStartup(MAKEWORD(2, 2), &(this->wsa_data)) != 0)
        {
            std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
            return false;
        }

        std::cout << "Initialised." << std::endl;
        // bool Cerver::initialized = true;
        return true;
    }

    static WSAData wsa_data; // Windows socket initialization
    static bool initialized;
#endif // _WIN32

public:
    /**
     * @brief Construct a new Cerver object
     * 
     * @param port The port the server will listen to
     */
    Cerver(unsigned int port)
    {
#if _WIN32
        if (this->initialized == false)
            this->initialize_winsocket();
#endif // _WIN32

        this->port = port;
        this->socket = create_socket();
        this->address = create_address();

#if _WIN32
        int bind_result = bind(this->socket, (struct sockaddr *)this->address, sizeof(struct sockaddr_in));
        if (bind_result == SOCKET_ERROR)
            std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
#else // _WIN32
        bind(this->socket, (struct sockaddr *)this->address, sizeof(struct sockaddr_in));
#endif // else _WIN32

        if (::listen(this->socket, BACKLOG) < 0)
        {
            printf("Error: The server is not listening.\n");
            exit(EXIT_FAILURE);
        }

        show_status();
    }

    /**
     * @brief Destroy the Cerver object
     * 
     */
    ~Cerver()
    {
        this->close_connection();
        free(address);
        address = NULL;
    }

#if _WIN32
    /**
     * @brief Interrupts communication with client and close server
     * 
     */
    void close_connection()
    {
        closesocket(this->socket);
        WSACleanup();
    }
#else // _WIN32
    /**
     * @brief Interrupts communication with client and close server
     * 
     */
    void close_connection()
    {
        ::close(this->socket);
    }
#endif // else _WIN32

    /**
     * @brief Defines a function to be executed when detected a new client.
     * Required when processing requests manually (instead of using a
     * HttpRouter, for instance)
     * 
     * @param handler_function The function that will process the request
     */
    void process_requests_with(bool (*handler_function)(Request *request))
    {
        this->requests_handler = handler_function;
    }

    /**
     * @brief Starts the server, listening to connections and sending identified
     * clients to be processed with the defined handler
     * 
     */
    void start()
    {
        Stopwatch request_timer = Stopwatch();

        while (true)
        {
            Request request = Request(this->listen());
            request_timer.start();

            bool executed_successfully = (this->requests_handler)(&request);

            if (executed_successfully)
            {
                std::cout << "ALL OK" << std::endl;
                request.respond();
            }
            request_timer.finish();
            request_timer.print_elapsed_time();

            request_timer.reset_times();
        }
    }

    /**
     * @brief Starts the server, listening to the specified port, and process
     * requests through the received `router`
     * 
     * @param router The HttpRouter that will manage the requests flow
     */
    void start(HttpRouter *router)
    {
        if (router == NULL)
            std::cout << "Failed to start server. Router is null" << std::endl;

        Stopwatch request_timer = Stopwatch();

        while (true)
        {
            HttpRequest request = HttpRequest(this->listen());
            request_timer.start();

            request.receive_client_data();

            std::string uri = "blablaa";
            std::string http_method = "GET";

            HttpRequestHandler handler = router->get_handler(uri, http_method);

            bool executed_successfully = handler(&request);

            if (executed_successfully)
                std::cout << "ALL OK" << std::endl;
            else
            {
                std::cout << "ALL NOT OK" << std::endl;
                // TODO: Send response as HttpRequest
                // request.respond_internal_server_error();
            }
            request_timer.finish();
            request_timer.print_elapsed_time();
            request_timer.reset_times();
        }
    }
};

#if _WIN32
WSAData Cerver::wsa_data = {};
bool Cerver::initialized = false;
#endif // _WIN32

#endif // __CERVER_HEADER