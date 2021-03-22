#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/cerver.hpp"
#include "router.hpp"

struct ServerContext {
    int socket;
    struct sockaddr_in *address;
};

typedef struct ServerContext ServerContext;

void report(struct sockaddr_in *serverAddress);


bool request_handler(Request *request) {
    std::cout << "TUTS TUTs" << std::endl;
    request->append_file("index.html");
    return true;
}


int main(void) {
    Cerver server = Cerver(8001);

    Router router = Router();

    router.add_route("/", [](Request *request) -> bool {
        std::cout << "I can know what is this" << std::endl;
        request->append_file("index.html");
        return true;
    });

    router.add_route("/test", [](Request *request) -> bool {
        std::cout << "I can know what is this too (/test)" << std::endl;
        request->append_file("test.html");
        return true;
    });

    server.process_requests_with(&request_handler);

    server.start();

    // while(true) {
    //     Request request = server.listen();
    //     request.append_file("index.html");
    //     request.respond();
    // }

    return 0;
}
