// Declaration of Table class
//

#pragma once

#include "Philosopher.h"

#include <vector>

class Table
{
public:
  typedef std::vector<std::shared_ptr<Philosopher>> philosopher_vector_t;

public:
  Table(int philosophers, Logger& log);
  ~Table();

  philosopher_vector_t& get_philosophers() { return philosophers_; };
  std::size_t get_minimum_drink_count() const;

private:
  // This vector contains our philosophers.  Each behaves on its own
  philosopher_vector_t philosophers_;

  Logger& log_;
};