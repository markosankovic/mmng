
#include "ethercat.h"
#include <loguru.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <uwebsockets/App.h>

#include "main.h"
#include "server.h"
#include "soem_master.h"

const char *keyFileName = "./certs/mmng.local.key";
const char *crtFileName = "./certs/mmng.local.crt";

int main() {
  SoemMaster master;

  Server server(master, keyFileName, crtFileName);
  std::thread serverThread([&server]() { server.start(); });

  LOG_F(INFO, "Server is running in a separate thread.");

  // TODO: Implement periodic execution for all machines at regular intervals.

  serverThread.join();

  LOG_F(INFO, "Server has stopped!");

  return 0;
}
