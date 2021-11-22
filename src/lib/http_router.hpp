#ifndef __CERVER_ROUTER
#define __CERVER_ROUTER

#include <iostream>
#include <map>
#include <string>

#include "http_request.hpp"

typedef bool (*HttpRequestHandler)(HttpRequest *);

class HttpRouter
{
private:
    bool allow_csrf;
    std::map<std::string, bool (*)(HttpRequest *)> routes;

    void set_not_found_route()
    {
        this->routes["not_found"] = [](HttpRequest *request) -> bool
        {
            std::string body = "<body>The server Router couldn't find a suitable route for this request</body>";
            std::string title = "<title>ROUTE NOT FOUND</title>";
            std::string final_html = "<html>" 
                + title 
                + body 
                + "</html>";

            std::string ok_header = "HTTP/1.1 200 OK\r\n";
            std::string datetime_header = "Date: Sun, 10 Oct 2010 23:26:07 GMT\r\n";
            std::string server_header = "Server: Cerver/0.0.1 (macOS)\r\n";
            std::string last_modified_header = "Last-Modified: Sun, 26 Sep 2010 22:04:35 GMT\r\n";
            std::string etag_header = "ETag: \"45b6-834-49130cc1182c0\"\r\n";
            std::string accept_range = "Accept-Ranges: bytes\r\n";
            std::string content_length_header = "Content-Length: "+ std::to_string(final_html.length()) + "\r\n";
            std::string connection_status_header = "Connection: close\r\n";
            std::string content_type_header = "Content-Type: text/html\r\n";

            std::string composed_response = ok_header 
                + datetime_header 
                + server_header 
                + last_modified_header 
                + etag_header 
                + accept_range 
                + content_length_header 
                + connection_status_header 
                + content_type_header 
                + "\r\n"
                + final_html;

            request->respond(composed_response);
            return true;
        };
    }

public:
    HttpRouter()
    {
        this->allow_csrf = false;
        this->set_not_found_route();
    }

    HttpRouter(bool allow_csrf)
    {
        this->allow_csrf = allow_csrf;
        this->set_not_found_route();
    }

    bool add_route(std::string route_path, bool (*handler)(HttpRequest *request))
    {
        this->routes[route_path] = handler;
        return true;
    }

    unsigned int count_request_handlers()
    {
        return (unsigned int)this->routes.size();
    }

    HttpRequestHandler get_handler(std::string path, std::string http_method)
    {
        if (this->routes.find(path) != this->routes.end())
            return this->routes[path];
        else
            return this->routes["not_found"];
    }

    bool redirector(HttpRequestHandler handler)
    {
        return true;
    }
};

#endif // __CERVER_ROUTER