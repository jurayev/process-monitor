#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor(int id);
  double Utilization();

 private:
  int id_;
  double prev_idle_ = 0;
  double prev_non_idle_ = 0;
};

#endif