//////////////////////////////////////////////////////////////////////////
// Table.cpp
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// Implementation of the Table class
//  This is mainly a container for Philosophers and
//  contains utility functions to work with them.
//

#include "Table.h"

Table::Table(int philosophers, Logger& log)
  : philosophers_()
  , drink_counts_(philosophers, 0)
  , log_(log)
{
  for (int i = 0; i < philosophers; i++)
    philosophers_.emplace_back(std::make_shared<Philosopher>(i, log_));
}

void Table::start()
{
  // Walk through the philosophers_ and tell them all to start
  for (auto& philosopher : philosophers_)
    philosopher->start();
}

// IDrinkListener interface
void Table::report_drink(int id)
{
  if (id < 0 || id > drink_counts_.size())
    return;

  drink_counts_[id]++;
}

bool Table::wait_for_minimum_drink_count(int drink_minimum, long long max_wait_ms)
{
  if (drink_counts_.empty())
  {
    log_.log("Trying to wait with no registered drinkers.");
    return false;
  }

  auto start_time = std::chrono::system_clock::now();
  auto end_time = start_time + std::chrono::milliseconds(max_wait_ms);

  while (std::chrono::system_clock::now() < end_time)
  {
    if (get_minimum_drink_count() >= drink_minimum)
      return true;

    // OK... Sleep for a bit at try again
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Didn't crest the drink minimum
  return false;
}

std::size_t Table::get_minimum_drink_count() const
{
  if (drink_counts_.empty())
    return 0;

  // Start with the first guy
  std::size_t min_drinks = drink_counts_[0];

  // Look to see if anyone is lower
  for (std::size_t i = 1; i < drink_counts_.size(); i++)
    if (drink_counts_[i] < min_drinks)
      min_drinks = drink_counts_[i];

  return min_drinks;
}

