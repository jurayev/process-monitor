#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  std::vector<Processor>& Cpus();
  std::vector<Process>& Processes();
  float MemoryUtilization() const;
  long UpTime() const;
  int TotalProcesses() const;
  int RunningProcesses() const;
  std::string Kernel() const;
  std::string OperatingSystem() const;

 private:
  std::vector<Processor> cpu_cores_ = {};
  std::vector<Process> processes_ = {};
};

#endif