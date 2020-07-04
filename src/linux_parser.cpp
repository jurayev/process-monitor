#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), kFilterWhiteSpace, kFilterUnderScore);
      replace(line.begin(), line.end(), kFilterEquals, kFilterWhiteSpace);
      replace(line.begin(), line.end(), kFilterDoubleQuotes, kFilterWhiteSpace);
      std::stringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kFilterPrettyName) {
          replace(value.begin(), value.end(), kFilterUnderScore,
                  kFilterWhiteSpace);
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

std::string LinuxParser::Kernel() {
  std::string target = "";
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::stringstream linestream(line);
    for (int i = 0; i < 3; i++) {
      linestream >> target;
    }
  }
  stream.close();
  return target;
}

// TODO: Update this to use filesystem
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (all_of(filename.begin(), filename.end(), ::isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float mem_total = findValueByKey<float>(kFilterMemTotal, kMeminfoFilename);
  float mem_free = findValueByKey<float>(kFilterMemFree, kMeminfoFilename);
  return (mem_total - mem_free) / mem_total;
}

long LinuxParser::UpTime() {
  long suspend = 0;
  long idle = 0;
  std::string line = getFileContent<std::string>(kUptimeFilename);
  std::stringstream linestream(line);
  linestream >> suspend >> idle;
  return suspend + idle;
}

std::vector<double> LinuxParser::CpuUtilization(int id) {
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line;
  std::string value;
  std::vector<double> cpu_meters;
  if (stream.is_open()) {
    std::string cpu_name = kFilterCpu + std::to_string(id);
    while (getline(stream, line)) {
      if (line.rfind(cpu_name) == 0) {
        std::stringstream linestream(line);
        linestream >> value;
        while (linestream >> value) {
          cpu_meters.emplace_back(stod(value));
        }
        stream.close();
        return cpu_meters;
      }
    }
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  int processes = findValueByKey<int>(kFilterProcesses, kStatFilename);
  return processes;
}

int LinuxParser::RunningProcesses() {
  int running_processes =
      findValueByKey<int>(kFilterRunningProcesses, kStatFilename);
  return running_processes;
}

std::string LinuxParser::Command(int pid) {
  std::string command =
      getFileContent<std::string>(std::to_string(pid) + kCmdlineFilename);
  return command.length() > 50 ? command.substr(0, 50) + " ..." : command;
}

std::string LinuxParser::Ram(int pid) {
  std::string ram = findValueByKey<std::string>(
      kFilterProcMem, std::to_string(pid) + kStatusFilename);
  return ram.empty() ? "0" : ram;
}

std::string LinuxParser::User(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string username;
  if (stream.is_open()) {
    std::string uid = findValueByKey<std::string>(
        kFilterUID, std::to_string(pid) + kStatusFilename);
    std::ifstream passwd_stream(kPasswordPath);
    std::string line;
    std::string value;
    if (passwd_stream.is_open()) {
      while (getline(passwd_stream, line)) {
        replace(line.begin(), line.end(), kFilterColumn, kFilterWhiteSpace);
        std::stringstream linestream(line);
        std::string password;
        std::string passwd_uid;
        linestream >> username >> password >> passwd_uid;
        if (uid == passwd_uid) {
          stream.close();
          passwd_stream.close();
          return username;
        }
      }
    }
    stream.close();
  }
  return "";
}

long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  std::string value;
  if (stream.is_open()) {
    getline(stream, line);
    std::stringstream linestream(line);
    for (int index = 0; index < 22; index++) {
      linestream >> value;
    }
    stream.close();
    return UpTime() - stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

std::vector<long> LinuxParser::ProcessCpuUtilization(int id) {
  std::ifstream stream(kProcDirectory + std::to_string(id) + kStatFilename);
  std::string line;
  std::string value;
  std::vector<long> cpu_meters = {};
  if (stream.is_open()) {
    getline(stream, line);
    std::stringstream linestream(line);
    for (int index = 0; index < 17; index++) {
      linestream >> value;
      if (index > 12 && index < 17) {
        cpu_meters.emplace_back(stol(value));
      }
    }
  }
  stream.close();
  return cpu_meters;
}