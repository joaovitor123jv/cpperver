#pragma once

#include <iostream>
#include <map>
#include <string>

#include "lib/request.hpp"

typedef bool (*RequestHandler) (Request *);


class Router {
    private:
        bool allow_csrf = false;
        std::map<std::string, bool (*)(Request *)> routes;

    public:

        Router() {
            this->routes["not_found"] = [](Request *request) -> bool {
                request->respond("ROUTE NOT FOUND");
                return true;
            };
        }

        bool add_route(std::string route_path, bool (*handler)(Request *request)) {
            this->routes[route_path] = handler;
            return true;
        }

        RequestHandler get_handler(std::string path, std::string http_method) {
            if (this->routes.find(path) != this->routes.end()) {
                return this->routes[path];
            } else {
                return this->routes["not_found"];
            }
        }

        bool redirector(RequestHandler handler) {
            return true;
        }
};