
#include "ethercat.h"
#include <loguru.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <uwebsockets/App.h>

#include "main.h"
#include "server.h"
#include "soem-master.h"

const char *ifname = "enx1c1adff64fae";

const char *keyFileName =
    "/home/marko/github/markosankovic/mmng/certs/mmng.key";
const char *crtFileName =
    "/home/marko/github/markosankovic/mmng/certs/mmng.crt";

int main() {
  SoemMaster master;
  master.init(ifname);

  Server server(master, keyFileName, crtFileName);
  std::thread serverThread([&server]() { server.start(); });

  LOG_F(INFO, "uWebSockets HTTP Server is running in a separate thread.");

  serverThread.join();

  LOG_F(INFO, "uWebSockets HTTP Server Stopped!");

  return 0;
}
