#include <iostream>
#include <soem_utils.h>

int main() {
  auto t = ethercat_slave_state_to_string(2);
  std::cout << t << std::endl;
  return 0;
}
