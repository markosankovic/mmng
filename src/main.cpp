#include <fmt/core.h>
#include <uwebsockets/App.h>

int main()
{
    uWS::App()
        .get("/", [](auto *res, auto *req)
             {
            res->writeHeader("Content-Type", "text/html; charset=utf-8");
            res->end("<h1>Welcome to uWebSockets HTTP Server</h1>"); })
        .listen(9000, [](auto *token) {
            if (token) {
                std::cout << "Server is listening on port 9000" << std::endl;
            } else {
                std::cerr << "Failed to start server!" << std::endl;
            }
        })
        .run();
    fmt::print("uWebSockets HTTP Server Stopped!\n");
    return 0;
}
