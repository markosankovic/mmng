#include "main.h"
#include "ethercat.h"
#include "soem-master.h"
#include <loguru.h>
#include <uwebsockets/App.h>

const char *ifname = "enx1c1adff64fae";

int main() {
  SoemMaster master;
  master.init(ifname);

  uWS::SSLApp({.key_file_name = "mmng.key", .cert_file_name = "mmng.crt"})
      .get("/",
           [](auto *res, auto *req) {
             res->writeHeader("Content-Type", "text/html; charset=utf-8");
             res->end("<h1>Welcome to uWebSockets HTTP Server</h1>");
           })
      .listen(9000,
              [](auto *listenSocket) {
                if (listenSocket) {
                  LOG_F(INFO, "Server is listening on port 9000");
                } else {
                  LOG_F(INFO, "Failed to start server!");
                }
              })
      .run();

  LOG_F(INFO, "uWebSockets HTTP Server Stopped!");

  ec_close();

  return 0;
}
