#include "main.h"
#include "ethercat.h"
#include "soem-master.h"
#include <loguru.h>
#include <uwebsockets/App.h>

#include <nlohmann/json.hpp>

const char *ifname = "enx1c1adff64fae";

const char *key_file_name =
    "/home/marko/github/markosankovic/mmng/certs/mmng.key";
const char *cert_file_name =
    "/home/marko/github/markosankovic/mmng/certs/mmng.crt";

int main() {
  SoemMaster master;
  master.init(ifname);

  uWS::SSLApp(
      {.key_file_name = key_file_name, .cert_file_name = cert_file_name})
      .get("/",
           [](auto *res, auto *req) {
             res->writeHeader("Content-Type", "text/html; charset=utf-8");
             res->end("<h1>Welcome to uWebSockets HTTP Server</h1>");
           })
      .get("/slaves",
           [&master](auto *res, auto *req) {
             nlohmann::json slaves;
             for (const auto &ptr : master.slaves) {
               slaves.push_back(ptr->get_info());
             }
             res->writeHeader("Content-Type", "application/json");
             res->end(slaves.dump());
           })
      .get("/slaves/:id",
           [&master](auto *res, auto *req) {
             std::string_view idv = req->getParameter(0);
             std::string id_str(idv.substr(0, idv.length()));
             uint32_t id = std::stoi(id_str);
             auto slave_info = master.slaves.at(id)->get_info();
             res->writeHeader("Content-Type", "application/json");
             nlohmann::json info = slave_info;
             res->end(info.dump());
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

  return 0;
}
