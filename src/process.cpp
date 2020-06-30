#include "process.h"

#include <unistd.h>

#include <cctype>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;
using namespace LinuxParser;

Process::Process(int id) : pid_(id) {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const {
  std::vector<long> cpu_meters = LinuxParser::ProcessCpuUtilization(pid_);
  long total_time = std::accumulate(cpu_meters.begin(), cpu_meters.end(), 0);
  long seconds = UpTime();
  seconds = seconds == 0 ? 1 : seconds;
  float cpu_utilization =
      ((float)total_time / (float)sysconf(_SC_CLK_TCK)) / (float)seconds;
  return cpu_utilization;
}

string Process::Command() const { return LinuxParser::Command(pid_); }

int Process::Ram() const { return std::stof(LinuxParser::Ram(pid_)) / 1024.0; }

string Process::User() const { return LinuxParser::User(pid_); }

long Process::UpTime() const {
  return LinuxParser::UpTime(pid_);
}

bool Process::operator<(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}