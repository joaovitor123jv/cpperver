#ifndef __CERVER_ROUTER
#define __CERVER_ROUTER

#include <iostream>
#include <map>
#include <string>

#include "http_request.hpp"

typedef bool (*HttpRequestHandler) (HttpRequest *);


class HttpRouter {
    private:
        bool allow_csrf;
        std::map<std::string, bool (*)(HttpRequest *)> routes;

        void set_not_found_route() {
            this->routes["not_found"] = [](HttpRequest *request) -> bool {
                request->respond("ROUTE NOT FOUND");
                return true;
            };
        }
 
    public:

        HttpRouter() {
            this->allow_csrf = false;
            this->set_not_found_route();
        }

        HttpRouter(bool allow_csrf) {
            this->allow_csrf = allow_csrf;
            this->set_not_found_route();
        }

        bool add_route(std::string route_path, bool (*handler)(HttpRequest *request)) {
            this->routes[route_path] = handler;
            return true;
        }

        unsigned int count_request_handlers() {
            return (unsigned int)this->routes.size();
        }

        HttpRequestHandler get_handler(std::string path, std::string http_method) {
            if (this->routes.find(path) != this->routes.end()) return this->routes[path];
            else return this->routes["not_found"];
        }

        bool redirector(HttpRequestHandler handler) {
            return true;
        }
};

#endif // __CERVER_ROUTER