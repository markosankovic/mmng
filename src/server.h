#pragma once

#include <uwebsockets/App.h>

#include "master.h"

class Server {
  Master &master;
  const char *keyFileName;
  const char *crtFileName;

public:
  Server(Master &master, const char *keyFileName, const char *crtFileName);

  void start();

  void configureCors(uWS::HttpResponse<true> *res);
};
