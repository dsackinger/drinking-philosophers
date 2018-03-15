// Declaration of Philosopher
//

#pragma once

#include "INeighbor.h"

#include "Logger.h"

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class Philosopher
  : public std::enable_shared_from_this<Philosopher>
  , public INeighbor
{
public:
  enum bottle_state {tranquil, thirsty, drinking};

  struct bottle_state_t
  {
    bool bot;   // Do we hold the bottle
    bool reqb;  // Do we hold the request token for the bottle
    bool need;  // Do we need the bottle
    bool dirty; // Is the fork clean
    std::weak_ptr<INeighbor> neighbor;
  };

  typedef std::map<int, bottle_state_t> bottle_state_map_t;

public:
  Philosopher(int id, Logger& log);
  virtual ~Philosopher();

public:
  std::size_t get_drink_count() const { return drink_count_; };
  void quit();

public:
  // INeighbor interface
  int get_id() override { return id_; };
  void introduce_neighbor(std::shared_ptr<INeighbor> neighbor) override;
  void send_bottle(int sender_id, bool dirty) override;
  void send_request(int sender_id) override;
  bool has_bottle(int id) override;
  bool has_request(int id) override;

private:
  void check_bottle_requests();

  void on_tranquil();
  void on_thirsty();
  void on_drinking();
  void work();

private:
  int id_;
  bottle_state state_;
  bottle_state_map_t bottles_;

  std::atomic<std::size_t> drink_count_;

  Logger& log_;
  std::atomic<bool> quit_;
  std::thread worker_;

private:
  Philosopher(const Philosopher& rhs) = delete;
  Philosopher& operator =(const Philosopher& rhs) = delete;

  // Disabling the move operator because you can't move an
  // object with a running thread.
  Philosopher(const Philosopher&& rhs) = delete;
};

