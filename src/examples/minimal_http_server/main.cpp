#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/cerver.hpp"
#include "lib/http_router.hpp"

int main(void) {
    Cerver server = Cerver(8001);

    HttpRouter router = HttpRouter();

    router.add_route("/", [](HttpRequest *request) -> bool {
        std::cout << "I can know what is this" << std::endl;
        request->append_file("index.html");
        return true;
    });

    router.add_route("/test", [](HttpRequest *request) -> bool {
        std::cout << "I can know what is this too (/test)" << std::endl;
        request->append_file("test.html");
        return true;
    });

    server.start(&router);

    return 0;
}