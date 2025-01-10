#include "master.h"

class Server {
  const Master &master;
  const char *keyFileName;
  const char *crtFileName;

public:
  Server(Master &master, const char *keyFileName, const char *crtFileName);

  void start();
};
