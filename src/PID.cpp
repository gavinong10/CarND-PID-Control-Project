#include "PID.h"
#include <algorithm>    // std::max
#include <chrono>
#include <iostream>
using namespace std;

/*
 * TODO: Complete the PID class.
 */

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  d_error = i_error = p_error = 0;

  last_time = std::chrono::system_clock::now().time_since_epoch() /
    std::chrono::milliseconds(1);
}

void PID::UpdateError(double cte) {
  unsigned long current_time =
    std::chrono::system_clock::now().time_since_epoch() /
    std::chrono::milliseconds(1);
  long delta_ts_ms = current_time - last_time;
  last_time = current_time;

  d_error = (cte - p_error) / delta_ts_ms*1000;
  i_error += p_error;
  p_error = cte;

}

double PID::GetSteer() {
  double steer = TotalError();
  steer = max(steer, -1.);
  steer = min(steer, 1.);
  return steer;
}

double PID::TotalError() {
  return -Kp * p_error + -Ki * i_error - Kd * d_error;
}