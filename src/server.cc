#include <loguru.h>
#include <nlohmann/json.hpp>
#include <uwebsockets/App.h>

#include "server.h"

Server::Server(Master &master, const char *keyFileName, const char *crtFileName)
    : master(master), keyFileName(keyFileName), crtFileName(crtFileName) {}

void Server::start() {
  uWS::SSLApp({.key_file_name = keyFileName, .cert_file_name = crtFileName})
      .get("/",
           [](auto *res, auto *req) {
             res->writeHeader("Content-Type", "text/html; charset=utf-8");
             res->end(
                 "<h1>Welcome to Motion Master: The Next Generation </h1>");
           })
      .get("/master/init",
           [&](auto *res, auto *req) {
             const char *ifname = "enx1c1adff64fae";
             master.init(ifname);
             res->writeHeader("Access-Control-Allow-Origin", "*");
             res->writeHeader("Access-Control-Allow-Methods",
                              "GET, POST, PUT, DELETE, OPTIONS");
             res->writeHeader("Access-Control-Allow-Headers",
                              "Content-Type, Authorization");
             res->writeHeader("Content-Type", "application/json");
             res->end("{}");
           })
      .get("/master/deinit",
           [&](auto *res, auto *req) {
             master.deinit();
             res->writeHeader("Access-Control-Allow-Origin", "*");
             res->writeHeader("Access-Control-Allow-Methods",
                              "GET, POST, PUT, DELETE, OPTIONS");
             res->writeHeader("Access-Control-Allow-Headers",
                              "Content-Type, Authorization");
             res->writeHeader("Content-Type", "application/json");
             res->end("{}");
           })
      .get("/slaves",
           [&](auto *res, auto *req) {
             nlohmann::json slaves;
             for (const auto &ptr : master.slaves) {
               slaves.push_back(ptr->get_info());
             }
             res->writeHeader("Access-Control-Allow-Origin", "*");
             res->writeHeader("Access-Control-Allow-Methods",
                              "GET, POST, PUT, DELETE, OPTIONS");
             res->writeHeader("Access-Control-Allow-Headers",
                              "Content-Type, Authorization");
             res->writeHeader("Content-Type", "application/json");
             res->end(slaves.dump());
           })
      .get("/slaves/:id",
           [&](auto *res, auto *req) {
             std::string_view idv = req->getParameter(0);
             std::string id_str(idv.substr(0, idv.length()));
             uint32_t id = std::stoi(id_str);
             auto slave_info = master.slaves.at(id)->get_info();
             res->writeHeader("Content-Type", "application/json");
             nlohmann::json info = slave_info;
             res->end(info.dump());
           })
      .get("/slaves/:id/set-state/:state",
           [&](auto *res, auto *req) {
             std::string_view idv = req->getParameter(0);
             std::string id_str(idv.substr(0, idv.length()));
             uint32_t id = std::stoi(id_str);

             std::string_view statev = req->getParameter(1);
             std::string state_str(statev.substr(0, statev.length()));
             uint32_t state = std::stoi(state_str);

             auto success = master.slaves.at(id)->set_state(state);
             res->writeHeader("Content-Type", "application/json");
             nlohmann::json info = {{"success", success}};
             res->end(info.dump());
           })
      .get("/slaves/:id/load-parameters",
           [&](auto *res, auto *req) {
             std::string_view idv = req->getParameter(0);
             std::string id_str(idv.substr(0, idv.length()));
             uint32_t id = std::stoi(id_str);

             master.slaves.at(id)->loadParameters();
             res->writeHeader("Content-Type", "application/json");
             nlohmann::json info = {{"success", true}};
             res->end(info.dump());
           })
      .get("/slaves/:id/upload/:index/:subindex",
           [&](auto *res, auto *req) {
             std::string_view idv = req->getParameter(0);
             std::string id_str(idv.substr(0, idv.length()));
             uint32_t id = std::stoi(id_str);

             std::string_view indexv = req->getParameter(1);
             std::string index_str(indexv.substr(0, indexv.length()));
             uint32_t index = std::stoi(index_str);

             std::string_view subindexv = req->getParameter(2);
             std::string subindex_str(subindexv.substr(0, subindexv.length()));
             uint32_t subindex = std::stoi(subindex_str);

             auto value = master.slaves.at(id)->upload(index, subindex);
             res->writeHeader("Content-Type", "application/json");
             nlohmann::json valueJson = {{"success", value}};
             res->end(valueJson.dump());
           })
      .listen(9000,
              [](auto *listenSocket) {
                if (listenSocket) {
                  LOG_F(INFO, "Server is listening on port 9000.");
                } else {
                  LOG_F(INFO, "Failed to start server!");
                }
              })
      .run();
}
