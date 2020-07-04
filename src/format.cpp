#include "format.h"

#include <string>

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::Time(long time) {
  char buffer[100];
  int hours = time / (60 * 60);
  int minutes = (time / 60) % 60;
  int seconds = time % 60;
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return buffer;
}