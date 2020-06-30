#include "format.h"

#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  // time_t elapsed_time = seconds;
  // tm *elapsed_tm;
  // elapsed_tm = localtime(&elapsed_time);
  // char buffer[100];
  // strftime(buffer, sizeof(buffer), "%H:%M:%S", elapsed_tm);
  // return buffer;
  char buffer[100];
  int m = seconds % 3600 / 60;
  int h = seconds / 3600;
  int s = seconds % 3600 % 60;
  sprintf(buffer, "%02d:%02d:%02d", h, m, s);
  return buffer;
}