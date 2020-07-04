#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {

// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// Filters
const std::string kFilterProcesses("processes");
const std::string kFilterRunningProcesses("procs_running");
const std::string kFilterMemTotal("MemTotal:");
const std::string kFilterMemFree("MemFree:");
const std::string kFilterCpu("cpu");
const std::string kFilterUID("Uid:");
const std::string kFilterProcMem("VmData:");
const std::string kFilterPrettyName("PRETTY_NAME");
const char kFilterWhiteSpace(' ');
const char kFilterUnderScore('_');
const char kFilterEquals('=');
const char kFilterDoubleQuotes('"');
const char kFilterColumn(':');

// Helpers
template <typename T>
T findValueByKey(std::string const& keyFilter, std::string const& fileName) {
  T value;
  std::string line;
  std::string key;

  std::ifstream stream(kProcDirectory + fileName);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::stringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          stream.close();
          return value;
        }
      }
    }
  }
  return value;
}

template <typename T>
T getFileContent(std::string const& filename) {
  std::string line;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    getline(stream, line);
    std::stringstream linestream(line);
    linestream >> value;
  }
  stream.close();
  return value;
};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_ = 1,
  kSystem_ = 2,
  kIdle_ = 3,
  kIOwait_ = 4,
  kIRQ_ = 5,
  kSoftIRQ_ = 6,
  kSteal_ = 7,
  kGuest_ = 8,
  kGuestNice_ = 9
};
std::vector<double> CpuUtilization(int id);

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string User(int pid);
long UpTime(int pid);
std::vector<long> ProcessCpuUtilization(int id);
};  // namespace LinuxParser

#endif