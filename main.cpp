//////////////////////////////////////////////////////////////////////////
// main.cpp
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// This is the entry point:
//  * Processes command line options
//  * Sets up the table
//  * Runs the test
//

#include "Philosopher.h"
#include "Table.h"

void test_two_philosophers(Logger& log)
{
  log.log("Testing two philosophers");

  auto p1 = std::make_shared<Philosopher>(1, log);
  auto p2 = std::make_shared<Philosopher>(2, log);

  // Now introduce them
  p1->introduce_neighbor(p2);

  if (p1->has_bottle(p2->get_id()) == p2->has_bottle(p1->get_id()))
  {
    log.log("Both philosophers disagree on who has the bottle.");
    ::exit(2);
  }

  if (p1->has_request(p2->get_id()) == p2->has_request(p1->get_id()))
  {
    log.log("Both philosophers disagree on who has the request.");
    ::exit(3);
  }

  log.log("Philosophers split the bottle and the request successfully.");
}

template<typename Rep, typename Period>
void test_neighbors_adjascent(int guest_count, int drink_count, std::chrono::duration<Rep, Period> max_wait, Logger& log)
{
  log.log("Starting test: test_neighbors_adjascent");
  // Set the guests at the table
  Table table(5, log);

  // Now introduce all philosophers to their neighbors
  std::vector<std::shared_ptr<Philosopher>>& guests = table.get_philosophers();
  for (std::size_t i = 1; i < guests.size(); i++)
  {
    guests[i]->introduce_neighbor(guests[i - 1]);
    guests[i]->introduce_neighbor(guests[(i + 1) % guests.size()]);
  }

  auto start = std::chrono::system_clock::now();
  table.start();

  while (table.get_minimum_drink_count() < 10 &&
    (std::chrono::system_clock::now() - start) < max_wait)
  {
    // Just let this thread sleep any time it comes up in the scheduling
    // and we aren't done
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  if (table.get_minimum_drink_count() < drink_count)
  {
    log.log("test_neighbors_adjascent failed to reach the drink count requirement.");
    return;
  }

  auto elapsed = std::chrono::system_clock::now() - start;
  log.log("test_neighbors_adjascent reached the drink count in ",
    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), "ms.");
}

template<typename Rep, typename Period>
void test_neighbors_all(int guest_count, int drink_count, std::chrono::duration<Rep, Period> max_wait, Logger& log)
{
  log.log("Starting test: test_neighbors_all");

  // Set the guests at the table
  Table table(5, log);

  // Now introduce all philosophers to their neighbors
  auto& guests = table.get_philosophers();
  for (std::size_t i = 1; i < guests.size(); i++)
    for (std::size_t j = 0; j < guests.size(); j++)
      if (i != j)
        guests[i]->introduce_neighbor(guests[j]);

  auto start = std::chrono::system_clock::now();
  table.start();

  while (table.get_minimum_drink_count() < 10 &&
    (std::chrono::system_clock::now() - start) < max_wait)
  {
    // Just let this thread sleep any time it comes up in the scheduling
    // and we aren't done
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  if (table.get_minimum_drink_count() < drink_count)
  {
    log.log("test_neighbors_adjascent failed to reach the drink count requirement.");
    return;
  }

  auto elapsed = std::chrono::system_clock::now() - start;
  log.log("test_neighbors_adjascent reached the drink count in ",
    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), "ms.");
}

int main()
{
  Logger log;
  log.log("Beginning tests....");

  // Start by making sure two philosophers can negotiate bottle/request
  test_two_philosophers(log);

  // Test with 5 guests - Thinking Drinking maxes out at 250ms,
  // so 10 drinks shouldn't take longer than 2.5 seconds.  Lets give them 5
  // to account for thread startup time
  test_neighbors_adjascent(5, 10, std::chrono::seconds(5), log);

  // Test with 5 guests with all being neighbors
  test_neighbors_all(5, 10, std::chrono::seconds(5), log);

  log.log("Tests Complete.");

  return 0;
}

