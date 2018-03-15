// Logger declaration
//

#pragma once

#include <mutex>
#include <sstream>

class Logger
{
public:
  struct entry
  {
    const std::chrono::system_clock::time_point time;
    const std::string line;
    entry * next;
  };

public:
  Logger() = default;
  virtual ~Logger() = default;

public:
  // Public logging function that takes any arguments as long as they output to a stream
  template<typename... Args>
  void log(Args... args)
  {
    std::stringstream ss;
    log(ss, args...);
    log(ss.str());
  }

private:
  // Final type for ending the recursion
  template<typename T>
  void log(std::stringstream& os, T final)
  {
    os << final;
  }

  // Expands to account for all types
  template<typename T, typename... Args>
  void log(std::stringstream& os, T current, Args... next)
  {
    os << current;
    log(os, next...);
  }

  // Logs the final message
  void log(const std::string& message);

private:
  std::mutex lock_;

private:
  // Disable assign / copy
  Logger(const Logger& rhs) = delete;
  Logger& operator =(const Logger& rhs) = delete;
};

