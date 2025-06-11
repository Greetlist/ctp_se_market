#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <string>

DEFINE_string(, , "");
DEFINE_string(, , "");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  return 0;
}
