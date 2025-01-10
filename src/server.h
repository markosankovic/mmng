#pragma once

#include "master.h"

class Server {
  Master &master;
  const char *keyFileName;
  const char *crtFileName;

public:
  Server(Master &master, const char *keyFileName, const char *crtFileName);

  void start();
};
