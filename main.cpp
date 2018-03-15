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

#include <iostream>

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
void run_test(int guest_count, int drink_count, bool ring, std::chrono::duration<Rep, Period> max_wait, Logger& log)
{
  log.log("Starting test.");
  log.log("Philosophers: ", guest_count);
  log.log("drink_count: ", drink_count);
  log.log("configuration: ", (ring ? "ring" : "all"));

  // Set the guests at the table
  Table table(guest_count, log);

  // Now introduce all philosophers to their neighbors
  auto& guests = table.get_philosophers();

  if (ring)
  {
    for (std::size_t i = 1; i < guests.size(); i++)
    {
      guests[i]->introduce_neighbor(guests[i - 1]);
      guests[i]->introduce_neighbor(guests[(i + 1) % guests.size()]);
    }
  }
  else // all configuration
  {
    for (std::size_t i = 1; i < guests.size(); i++)
      for (std::size_t j = 0; j < guests.size(); j++)
        if (i != j)
          guests[i]->introduce_neighbor(guests[j]);
  }

  auto start_time = std::chrono::system_clock::now();

  table.start();
  bool success = table.wait_for_minimum_drink_count(drink_count,
    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(5)).count());

  if (!success)
  {
    log.log("Failed to reach the drink count requirement.");
    return;
  }

  auto elapsed = std::chrono::system_clock::now() - start_time;
  log.log("Reached the drink count in ",
    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), "ms.");
}

int main(int argc, const char * argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: philo <philosophers> <drink_count> [all | ring]" << std::endl
      << "  philosophers - must specify at least 2 philosophers" << std::endl
      << "  drink_count - minimum number of drinks before exiting (5 minute limit)" << std::endl
      << std::endl
      << "  all  - philosophers coordinate with all neighbors" << std::endl
      << "  ring - philosophers only coordinate with adjacent neighbors" << std::endl;

    return 0;
  }

  int philosophers = ::atoi(argv[1]);
  int drink_count = ::atoi(argv[2]);

  bool ring = false;

  if (argc >= 3)
  {
    std::string arg = argv[2];
    ring = (arg.compare("ring"));
  }

  Logger log;
  // Start by making sure two philosophers can negotiate bottle/request
  test_two_philosophers(log);

  log.log("Beginning tests....");

  // Run the test
  run_test(philosophers, drink_count, ring, std::chrono::minutes(5), log);

  log.log("Tests Complete.");

  return 0;
}

