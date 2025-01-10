#include <loguru.h>
#include <nlohmann/json.hpp>
#include <uwebsockets/App.h>

#include "server.h"

Server::Server(Master &master, const char *keyFileName, const char *crtFileName)
    : master(master), keyFileName(keyFileName), crtFileName(crtFileName) {}

void Server::writeHeaders(uWS::HttpResponse<true> *res) {
  res->writeHeader("Access-Control-Allow-Origin", "*");
  res->writeHeader("Access-Control-Allow-Methods",
                   "GET, POST, PUT, DELETE, OPTIONS");
  res->writeHeader("Access-Control-Allow-Headers",
                   "Content-Type, Authorization");
  res->writeHeader("Content-Type", "application/json");
}

template <typename T> T Server::getParameter(uWS::HttpRequest *req, int index) {
  std::string_view idv = req->getParameter(index);
  std::string p(idv.substr(0, idv.length()));

  if constexpr (std::is_same<T, int>::value) {
    return std::stoi(p);
  } else if constexpr (std::is_same<T, float>::value) {
    return std::stof(p);
  } else if constexpr (std::is_same<T, double>::value) {
    return std::stod(p);
  } else {
    static_assert(
        false, "Unsupported type"); // Compile-time error for unsupported types
  }
}

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

             writeHeaders(res);
             res->end();
           })
      .get("/master/deinit",
           [&](auto *res, auto *req) {
             master.deinit();

             writeHeaders(res);
             res->end();
           })
      .get("/slaves",
           [&](auto *res, auto *req) {
             nlohmann::json slaves = nlohmann::json::array();
             for (const auto &ptr : master.slaves) {
               slaves.push_back(ptr->get_info());
             }

             writeHeaders(res);
             res->end(slaves.dump());
           })
      .get("/slaves/:id",
           [&](auto *res, auto *req) {
             auto id = getParameter<int>(req, 0);
             nlohmann::json info = master.slaves.at(id)->get_info();

             writeHeaders(res);
             res->end(info.dump());
           })
      .get("/slaves/:id/set-state/:state",
           [&](auto *res, auto *req) {
             auto id = getParameter<int>(req, 0);
             auto state = getParameter<int>(req, 1);

             master.slaves.at(id)->set_state(state);

             writeHeaders(res);
             res->end();
           })
      .get("/slaves/:id/load-parameters",
           [&](auto *res, auto *req) {
             auto id = getParameter<int>(req, 0);

             master.slaves.at(id)->loadParameters();

             writeHeaders(res);
             res->end();
           })
      .get("/slaves/:id/upload/:index/:subindex",
           [&](auto *res, auto *req) {
             auto id = getParameter<int>(req, 0);
             auto index = getParameter<int>(req, 1);
             auto subindex = getParameter<int>(req, 2);

             auto value = master.slaves.at(id)->upload(index, subindex);
             nlohmann::json valueJson = {{"success", value}};

             writeHeaders(res);
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
