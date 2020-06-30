#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor(int id);
  float Utilization();

 private:
  int id_;
  float prev_idle_ = 0;
  float prev_non_idle_ = 0;
};

#endif