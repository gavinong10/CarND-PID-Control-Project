#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#include <fstream>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  std::string str; 

  uWS::Hub h;

  PID pid;

  bool learn = false;

  bool running = false;
  
  time_t timer;
  time_t timer_last;

  // Get current time for reference
  time(&timer_last);
  // TODO: Initialize the pid variable.

  std::vector<double> p{0.19, 0, 0.14};
  std::vector<double> dp{0.02, 0.001, 0.01};
  double best_err = -1;
  double cum_error = 0;
  double speedCap = 15.;

  int BATCH_LEN_SECS = 5;
  int mode = -1;
  int i = 0;

// def twiddle(tol=0.2): 
//     p = [0, 0, 0]
//     dp = [1, 1, 1]
//     robot = make_robot()
//     x_trajectory, y_trajectory, best_err = run(robot, p)

//     it = 0
//     while sum(dp) > tol:
//         print("Iteration {}, best error = {}".format(it, best_err))
//         for i in range(len(p)):
//             p[i] += dp[i]
//             robot = make_robot()
//             x_trajectory, y_trajectory, err = run(robot, p)

//             if err < best_err:
//                 best_err = err
//                 dp[i] *= 1.1
//             else:
//                 p[i] -= 2 * dp[i]
//                 robot = make_robot()
//                 x_trajectory, y_trajectory, err = run(robot, p)

//                 if err < best_err:
//                     best_err = err
//                     dp[i] *= 1.1
//                 else:
//                     p[i] += dp[i]
//                     dp[i] *= 0.9
//         it += 1
//     return p

  if(!learn) {
    pid.Init(p[0], p[1], p[2]);
  }

  h.onMessage([&](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {

    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    

    if(!learn) {
      std::vector<double> p_candidate;
      int i = 0;
      bool isNew = false;
      std::ifstream file("../param.txt");
      while (std::getline(file, str))
      {
          double val = std::stod(str);
          p_candidate.push_back(val);
          
          if(p[i] != val) {
            isNew = true;
          }
          ++i;
      }
      if(isNew) {
        p = p_candidate;
        pid.Init(p[0], p[1], p[2]);
      }

      if (length && length > 2 && data[0] == '4' && data[1] == '2')
      {
        auto s = hasData(std::string(data).substr(0, length));
        if (s != "") {
          auto j = json::parse(s);
          std::string event = j[0].get<std::string>();
          if (event == "telemetry") {
            // j[1] is the data JSON object
            double cte = std::stod(j[1]["cte"].get<std::string>());
            double speed = std::stod(j[1]["speed"].get<std::string>());
            double angle = std::stod(j[1]["steering_angle"].get<std::string>());

            pid.UpdateError(cte);

            double error_value = pid.TotalError();
            double steer_value = pid.GetSteer();

            /*
            * TODO: Calcuate steering value here, remember the steering value is
            * [-1, 1].
            * NOTE: Feel free to play around with the throttle and speed. Maybe use
            * another PID controller to control the speed!
            */
            
            // DEBUG
            // std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

            json msgJson;
            msgJson["steering_angle"] = steer_value;
            msgJson["throttle"] = 0.3;
            auto msg = "42[\"steer\"," + msgJson.dump() + "]";
            // std::cout << msg << std::endl;
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);

            std::cout << "p = ";

            for (auto v: p) {
              std::cout << v << ' ';
            }
            
            std::cout << "\n" << std::endl;
          }
        } else {
          // Manual driving
          std::string msg = "42[\"manual\",{}]";
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      }
    } else {

      time(&timer);

      // Gavin TODO NOW: Ensure that we accumulate errors at fixed time intervals only

      if (length && length > 2 && data[0] == '4' && data[1] == '2')
      {
        auto s = hasData(std::string(data).substr(0, length));
        if (s != "") {
          auto j = json::parse(s);
          std::string event = j[0].get<std::string>();
          if (event == "telemetry") {
            // j[1] is the data JSON object
            double cte = std::stod(j[1]["cte"].get<std::string>());
            double speed = std::stod(j[1]["speed"].get<std::string>());
            double angle = std::stod(j[1]["steering_angle"].get<std::string>());

            pid.UpdateError(cte);

            double error_value = pid.TotalError();
            double steer_value = pid.GetSteer();

            // Only control if speed exceeds a certain value
            if(speed > 5) {
              // Only control if speed exceeds a certain value
              if(!running) {
                running = true;
                timer_last = timer;
                pid.Init(p[0], p[1], p[2]);
                cum_error = 0;
                i = 0;
                // Initialize with the appropriate PID values
                // Start a timer and run for 5 seconds
              }

              // accumulate error in run
              cum_error += error_value;

              double seconds_elapsed = difftime(timer,timer_last);
              if(seconds_elapsed >= BATCH_LEN_SECS) {
                running = false;
                if(best_err < 0 || cum_error < best_err) {
                  best_err = cum_error;
                }

                // Perform twiddle until we get 5 iterations in a row that does not change the error.
                // STILL TO DO... do the 5 iterations
                if(mode == -1) {
                  for(int i = 0; i < p.size(); ++i) {
                    p[i] += dp[i];
                  }
                  mode = 0;
                } else if(mode == 0) {
                    if(cum_error < best_err) {
                      best_err = cum_error;
                      dp[i] *= 1.1;
                      i = (i + 1) % p.size();
                    } else {
                      p[i] -= 2 * dp[i];
                      mode = 1;
                    }
                } else if(mode == 1) {
                  if(cum_error < best_err) {
                    best_err = cum_error;
                    dp[i] *= 1.1;
                  } else {
                    p[i] += dp[i];
                    dp[i] *= 0.9;
                  }
                  i = (i + 1) % p.size();
                  mode = -1;
                }
              }
              
              std::cout << "p = ";

              for (auto v: p) {
                std::cout << v << ' ';
              }
              
              std::cout << "\n" << std::endl;


  //         for i in range(len(p)):
  //             p[i] += dp[i]
  //             robot = make_robot()
  //             x_trajectory, y_trajectory, err = run(robot, p)

  //             if err < best_err:
  //                 best_err = err
  //                 dp[i] *= 1.1
  //             else:
  //                 p[i] -= 2 * dp[i]
  //                 robot = make_robot()
  //                 x_trajectory, y_trajectory, err = run(robot, p)

  //                 if err < best_err:
  //                     best_err = err
  //                     dp[i] *= 1.1
  //                 else:
  //                     p[i] += dp[i]
  //                     dp[i] *= 0.9
  //         it += 1
              // Once iterations have completed, reset the total error but keep running

              // Once the total error reset has completed 10 times, extract the PID parameters and bail

            } else {
              running = false;
              // Manual driving
              std::string msg = "42[\"manual\",{}]";
              ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
              return;
            }

            /*
            * TODO: Calcuate steering value here, remember the steering value is
            * [-1, 1].
            * NOTE: Feel free to play around with the throttle and speed. Maybe use
            * another PID controller to control the speed!
            */
            
            // DEBUG
            // std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

            json msgJson;
            msgJson["steering_angle"] = steer_value;
            if (speed < speedCap) {
              msgJson["throttle"] = 0.3;
            } else {
              msgJson["throttle"] = 0.;
            }
            
            auto msg = "42[\"steer\"," + msgJson.dump() + "]";
            // std::cout << msg << std::endl;
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
          }
        } else {
          // Manual driving
          std::string msg = "42[\"manual\",{}]";
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
