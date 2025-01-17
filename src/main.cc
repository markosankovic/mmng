
#include "ethercat.h"
#include <loguru.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <uwebsockets/App.h>

#include "main.h"
#include "server.h"
#include "soem-master.h"

const char *keyFileName = "./certs/mmng.key";
const char *crtFileName = "./certs/mmng.crt";

int main() {
  SoemMaster master;

  Server server(master, keyFileName, crtFileName);
  std::thread serverThread([&server]() { server.start(); });

  LOG_F(INFO, "Server is running in a separate thread.");

  serverThread.join();

  LOG_F(INFO, "Server has stopped!");

  return 0;
}
