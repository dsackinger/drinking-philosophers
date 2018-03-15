//////////////////////////////////////////////////////////////////////////
// Philosopher.cpp
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// Implementation of the Philosopher class
//  This implements the specification found at:
//  https://www.cs.utexas.edu/users/misra/scannedPdf.dir/DrinkingPhil.pdf
//

#include "Philosopher.h"

#include <vector>

// Start all philosophers in state of tranquil
Philosopher::Philosopher(int id, Logger& log, IDrinkListener * listener)
  : id_(id)
  , state_(tranquil)
  , bottles_()
  , listener_(listener)
  , log_(log)
  , quit_(false)
  , start_(false)
  , worker_(std::thread(&Philosopher::work, this))
{
  // Initialize the randomizer
  ::srand(static_cast<unsigned int>(::time(NULL)));
}

Philosopher::~Philosopher()
{
  quit();
}

void Philosopher::start()
{
  std::unique_lock<std::mutex> lock(start_lock_);
  start_ = true;
  start_cv_.notify_all();
}

void Philosopher::quit()
{
  quit_ = true;

  if (worker_.joinable())
    worker_.join();
};

// INeighbor interface
void Philosopher::introduce_neighbor(std::shared_ptr<INeighbor> neighbor)
{
  auto id = neighbor->get_id();

  // Remove invalid cases of introduction to self or duplicate ids
  if (id == id_)
  {
    // Introduction to self
    if (this == neighbor.get())
      return;
    else
    {
      log_.log("Error: Guest with duplicate ID detected!");
      std::exit(1);
    }
  }

  // See if we already have a bottle for this neighbor
  if (bottles_.find(id) != bottles_.end())
    return;

  // Set up our bottle for this neighbor
  // Assume we have the token and we will make
  // sure to send the bottle to the neighbor
  // If he doesn't know us yet, he will drop the bottle but will
  // then introduce himself back to us and give us the bottle
  // Forks are assumed dirty until someone drinks
  bottles_[id] = { false, true, false, false, neighbor };

  // Try to send the neighbor the bottle.  If he doesn't know us
  // yet, he should discard the request.
  neighbor->send_bottle(id_, false);

  // Introduce ourselves to the other neighbor.  If we were the receiving side,
  // he will recognize us and drop the request
  neighbor->introduce_neighbor(shared_from_this());

  // The neighbor may have given us the bottle back during introductions
  auto& bottle = bottles_[id];
  if (bottle.bot && bottle.reqb)
  {
    // We have both.  Drop the request as we can safely assume the
    // neighbor dropped the bottle
    bottle.reqb = false;
  }
}

void Philosopher::send_bottle(int sender_id, bool dirty)
{
  std::unique_lock<std::mutex> lock(bottles_lock_);

  // Here we are receiving the request from a neighbor

  // (R4) Receive a Bottle:
  //    upon receiving bottle b ->
  //    bot(b) := true
  auto entry = bottles_.find(sender_id);
  if (entry == bottles_.end())
    return;

  // We now own the bottle
  auto& bottle = entry->second;
  bottle.bot = true;
  bottle.dirty = dirty;
}

void Philosopher::send_request(int sender_id)
{
  std::unique_lock<std::mutex> lock(bottles_lock_);
  // Here we are receiving the request token from a neighbor

  // (R3) Receive Request for a Bottle:
  //    upon receiving request for bottle b ->
  //    reqb(b) := true;
  auto entry = bottles_.find(sender_id);
  if (entry == bottles_.end())
    return;

  // We now own the request token
  auto& bottle = entry->second;
  bottle.reqb = true;
}

bool Philosopher::has_bottle(int id)
{
  std::unique_lock<std::mutex> lock(bottles_lock_);
  // Look up the bottle record
  auto entry = bottles_.find(id);
  return (entry != bottles_.end() && entry->second.bot);
}

bool Philosopher::has_request(int id)
{
  std::unique_lock<std::mutex> lock(bottles_lock_);
  // Look up the bottle record
  auto entry = bottles_.find(id);
  return (entry != bottles_.end() && entry->second.reqb);
}

// State machine functions
void Philosopher::on_tranquil()
{
  // Normally, we would have a timed operation here that
  // takes a consistent time to complete.  For now, lets just
  // yield to make sure other threads get a chance and then let
  // the transition to thirsty take place.

  // If we were going to wait a longer time, we would need to
  // service the check_bottle_requests().  Ideally,
  // check_bottle_requests() would be on a background thread.

  // Transition to being thirsty
  state_ = thirsty;

  { // Scope for lock
    std::unique_lock<std::mutex> lock(bottles_lock_);

    // To fully implement the specification, we would allow
    // a variable subset of the bottles to be needed at any
    // time.  For this simple test, just request all the
    // bottles

    // Mark all bottles as needed in order to drink
    for (auto& entry : bottles_)
      entry.second.need = true;
  }
}

void Philosopher::on_thirsty()
{
  std::vector<std::shared_ptr<INeighbor>> requests;

  { // Scope for lock
    std::unique_lock<std::mutex> lock(bottles_lock_);

    // See if we need to send any requests
    for (auto& bottle_entry : bottles_)
    {
      auto id = bottle_entry.first;
      auto& bottle = bottle_entry.second;

      // (R1) Request a Bottle:
      //   thirsty, need(b), reqb(b), ~bot(b) -> Send request for bottle B
      //   reqb(b) := false
      if (bottle.need && bottle.reqb && !bottle.bot)
      {
        auto neighbor = bottle.neighbor.lock();
        if (!neighbor)
        {
          // Neighbor has disappeared on us.  We should just remove this bottle.
          // For now, mark it as not needed
          bottle.need = false;
          continue;
        }

        requests.push_back(neighbor);
        bottle.reqb = false;
      }
    }
  }

  // Sending requests has to be done outside of the lock
  // to avoid deadlocks
  for (auto neighbor : requests)
    neighbor->send_request(id_);

  { // Scope for lock
    std::unique_lock<std::mutex> lock(bottles_lock_);

    // If we find any bottles that we need but do not have, we
    // cannot move into a drinking state
    for (auto& bottle_entry : bottles_)
    {
      auto& bottle = bottle_entry.second;
      if (bottle.need && !bottle.bot)
        return;
    }
  }

  // We have all bottles.  Move to a drinking state
  state_ = drinking;
}

void Philosopher::on_drinking()
{
  // We are in the drinking state.  Lets drink for a set time and change our state
  log_.log("Philosopher[", id_, "] is drinking.");

  // Let the listener know we are taking a drink
  if (listener_)
    listener_->report_drink(id_);

  { // Scope for lock
    std::unique_lock<std::mutex> lock(bottles_lock_);

    // We no longer need any of the bottles as we have finished our drinking
    // Make sure to clean any forks as we are done drinking
    for (auto& entry : bottles_)
    {
      auto& bottle = entry.second;
      bottle.need = false;
      bottle.dirty = true;
    }
  }  // Scope for lock

  // Done drinking.  Change states
  state_ = tranquil;
}

// This function checks to see if we have any bottles to send to requesters
void Philosopher::check_bottle_requests()
{
  std::vector<std::shared_ptr<INeighbor>> requests;

  { // Scope for lock
    std::unique_lock<std::mutex> lock(bottles_lock_);

    // See if we need to give any bottles
    for (auto& bottle_entry : bottles_)
    {
      // (R2) Send a bottle:
      //    reqb(b), bot(b), ~[need(b) and (drinking or fork(f))] ->
      //    send bottle b;
      //    bot(b) := false
      auto id = bottle_entry.first;
      auto& bottle = bottle_entry.second;
      if (bottle.reqb && bottle.bot
        && !(bottle.need && (state_ == drinking || !bottle.dirty)))
      {
        // We need to send the bottle
        auto neighbor = bottle.neighbor.lock();
        if (!neighbor)
        {
          // Neighbor has disappeared on us.  For now, mark it unneeded
          bottle.need = false;
          continue;
        }

        requests.push_back(neighbor);
        bottle.bot = false;

        // Always clean the fork before sending the bottle
        bottle.dirty = false;
      }
    }
  }

  // All sending should be done when not holding the lock
  // to avoid deadlocks
  // Always send clean forks
  for (auto& neighbor : requests)
    neighbor->send_bottle(id_, false);
}


void Philosopher::work()
{
  std::unique_lock<std::mutex> lock(start_lock_);
  
  while (!start_ && !quit_)
    start_cv_.wait_for(lock, std::chrono::milliseconds(100));

  log_.log("Philosopher[", id_, "] is starting.");

  std::map<bottle_state, std::function<void()>> state_map = {
    { tranquil, std::bind(&Philosopher::on_tranquil, this) },
    { thirsty, std::bind(&Philosopher::on_thirsty, this) },
    { drinking, std::bind(&Philosopher::on_drinking, this) }
  };

  while (!quit_)
  {
    auto old_state = state_;
    state_map[state_]();

    // See if we need to give any bottles to our neighbors
    check_bottle_requests();

    // Give other threads a chance to run
    if (state_ == old_state)
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  log_.log("Philosopher[", id_, "] is exiting.");
}

