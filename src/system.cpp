#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

System::System() {
  for (int id = 0; id < sysconf(_SC_NPROCESSORS_ONLN); id++) {
    cpu_cores_.emplace_back(Processor(id));
  }
}

std::vector<Processor>& System::Cpus() { return cpu_cores_; }

std::vector<Process>& System::Processes() {
  processes_.clear();
  for (auto pid : LinuxParser::Pids()) {
    processes_.emplace_back(pid);
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() const { return LinuxParser::Kernel(); }

float System::MemoryUtilization() const {
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() const {
  return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

long System::UpTime() const { return LinuxParser::UpTime(); }