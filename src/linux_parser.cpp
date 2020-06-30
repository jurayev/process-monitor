#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string target = "";
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 3; i++) {
      linestream >> target;
    }
  }
  return target;
}

// TODO: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float memtotal;
  float memfree;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    // TODO: add complete utilization: usedmem, buffersmem, cachedmem etc.
    string memtotal_value = ScanValue(stream, "MemTotal:");
    memtotal = memtotal_value == "" ? 1.0 : stof(memtotal_value);
    string memfree_value = ScanValue(stream, "MemFree:");
    memfree = memfree_value == "" ? 1.0 : stof(memfree_value);
    return (memtotal - memfree) / memtotal;
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  string line;
  long suspend = 0;
  long idle = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    linestream >> suspend >> idle;
  }
  return suspend + idle;
}

vector<float> LinuxParser::CpuUtilization(int id) {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, value;
  vector<float> cpu_meters;
  if (stream.is_open()) {
    string cpu_name = "cpu" + std::to_string(id);
    while (getline(stream, line)) {
      if (line.rfind(cpu_name) == 0) {
        std::istringstream linestream(line);
        linestream >> value;
        while (linestream >> value) {
          cpu_meters.emplace_back(stof(value));
        }
        return cpu_meters;
      }
    }
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string value = ScanValue(stream, "processes");
    return value == "" ? 0 : stoi(value);
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string value = ScanValue(stream, "procs_running");
    return value == "" ? 0 : stoi(value);
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string command = "";
  if (stream.is_open()) {
    getline(stream, command);
  }
  return command;
}

string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string ram = ScanValue(stream, "VmSize:");
    return ram == "" ? "0" : ram;
  }
  return "0";
}

string LinuxParser::User(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string username;
  if (stream.is_open()) {
    string uid = ScanValue(stream, "Uid:");
    std::ifstream passwd_stream(kPasswordPath);
    string line, value;
    if (passwd_stream.is_open()) {
      while (getline(passwd_stream, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        string password, passwd_uid;
        linestream >> username >> password >> passwd_uid;
        if (uid == passwd_uid) {
          return username;
        }
      }
    }
  }
  return "";
}

long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line, value;
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    for (int index = 0; index < 22; index++) {
      linestream >> value;
    }
    return UpTime() - stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

vector<long> LinuxParser::ProcessCpuUtilization(int id) {
  std::ifstream stream(kProcDirectory + std::to_string(id) + kStatFilename);
  string line, value;
  vector<long> cpu_meters = {};
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    for (int index = 0; index < 17; index++) {
      linestream >> value;
      if (index > 12 && index < 17) {
        cpu_meters.emplace_back(stol(value));
      }
    }
  }
  return cpu_meters;
}

string LinuxParser::ScanValue(std::ifstream& stream, string target) {
  string line, key, value;
  while (getline(stream, line)) {
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == target) {
      return value;
    }
  }
  return "";
}