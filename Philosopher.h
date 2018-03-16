//////////////////////////////////////////////////////////////////////////
// Philosopher.h
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// Philosopher declaration:
//  This is the main actor in the Drinking Philosopher
//  solution.
//

#if !defined(__PHILOSOPHER_H__)
#define __PHILOSOPHER_H__

#include "INeighbor.h"
#include "IDrinkListener.h"
#include "Logger.h"

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

class Philosopher
  : public std::enable_shared_from_this<Philosopher>
  , public INeighbor
{
public:
  enum bottle_state {tranquil, thirsty, drinking};

  struct bottle_state_t
  {
    bool bot;   // Do we hold the bottle (and fork)
    bool reqb;  // Do we hold the request token for the bottle
    bool need;  // Do we need the bottle
    bool dirty; // Is the fork clean
    std::weak_ptr<INeighbor> neighbor;
  };

  typedef std::map<int, bottle_state_t> bottle_state_map_t;

public:
  Philosopher(int id, Logger& log, IDrinkListener * listener = nullptr);
  virtual ~Philosopher();

public:
  inline void set_listener(IDrinkListener * listener) { listener_ = listener; };
  inline void set_wait(bool wait) { wait_ = wait; };
  void start();
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
  std::mutex bottles_lock_;

  bool wait_;
  std::chrono::system_clock::time_point end_tranquil_;

  IDrinkListener * listener_;

  // To ensure a fair start
  std::atomic<bool> start_;
  std::mutex start_lock_;
  std::condition_variable start_cv_;

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

#endif // #if !defined(__PHILOSOPHER_H__)

