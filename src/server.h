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

  template <typename T> T getParameter(uWS::HttpRequest *req, int index);

  void writeHeaders(uWS::HttpResponse<true> *res);
};
