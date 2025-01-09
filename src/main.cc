#include "main.h"
#include "ethercat.h"
#include <loguru.h>
#include <uwebsockets/App.h>

const char *ifname = "enx1c1adff64fae";

int main() {
  // This prevents ecx_config_map_group from transitioning devices without
  // firmware installed into SAFE-OPERATIONAL state.
  // SOMANET devices without firmware that are transitioned into this state
  // become stuck and can only be recovered by power cycling the device.
  ecx_context.manualstatechange = TRUE;

  if (ec_init(ifname)) {
    LOG_F(INFO, "ec_init on %s succeeded.\n", ifname);
    if (ec_config_init(FALSE) > 0) {
      LOG_F(INFO, "%d slaves found and configured.\n", ec_slavecount);
    }
  }

  uWS::App()
      .get("/",
           [](auto *res, auto *req) {
             res->writeHeader("Content-Type", "text/html; charset=utf-8");
             res->end("<h1>Welcome to uWebSockets HTTP Server</h1>");
           })
      .listen(9000,
              [](auto *token) {
                if (token) {
                  LOG_F(INFO, "Server is listening on port 9000");
                } else {
                  LOG_F(INFO, "Failed to start server!");
                }
              })
      .run();
  LOG_F(INFO, "uWebSockets HTTP Server Stopped!");

  return 0;
}
