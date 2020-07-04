#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

using namespace LinuxParser;

Processor::Processor(int id) : id_(id) {}

double Processor::Utilization() {
  std::vector<double> cpu_meters = CpuUtilization(id_);

  double idle = cpu_meters[CPUStates::kIdle_] + cpu_meters[CPUStates::kIOwait_];
  double non_idle =
      cpu_meters[CPUStates::kUser_] + cpu_meters[kNice_] +
      cpu_meters[CPUStates::kSystem_] + cpu_meters[CPUStates::kIRQ_] +
      cpu_meters[CPUStates::kSoftIRQ_] + cpu_meters[CPUStates::kSteal_];

  double prev_total = prev_idle_ + prev_non_idle_;
  double total = idle + non_idle;

  double totald = total - prev_total;
  double idled = idle - prev_idle_;

  double cpu_usage = (totald - idled) / totald;
  // save current meters for calculating dynamic cpu utilization
  prev_idle_ = idle;
  prev_non_idle_ = non_idle;
  return cpu_usage;
}