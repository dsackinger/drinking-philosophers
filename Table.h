//////////////////////////////////////////////////////////////////////////
// Table.h
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// Table declaration:
//  This is a simple container for holding a group of
//  Philosopher objects.  Probably the most important
//  part is the utility function to figure out the lowest
//  number of drinks of anyone at the group
//

#if !defined(__TABLE_H__)
#define __TABLE_H__

#include "Philosopher.h"

#include <vector>

class Table
  : public IDrinkListener
{
public:
  typedef std::vector<std::shared_ptr<Philosopher>> philosopher_vector_t;

public:
  Table(int philosophers, Logger& log);
  virtual ~Table();

  void start();

  philosopher_vector_t& get_philosophers() { return philosophers_; };
  std::size_t get_minimum_drink_count() const;

  bool wait_for_minimum_drink_count(int drink_minimum, long long max_wait_ms);

public:
  // IDrinkListener interface
  void report_drink(int id) override;

private:
  // This vector contains our philosophers.  Each behaves on its own
  philosopher_vector_t philosophers_;
  std::vector<std::size_t> drink_counts_;

  Logger& log_;
};

#endif // #if !defined(__TABLE_H__)

