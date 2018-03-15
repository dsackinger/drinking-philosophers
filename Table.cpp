// Table implementation
//
// State Transitions:
//  tranquil => thirsty
//  thirsty => drinking
//  drinking => tranquil
//

#include "Table.h"

Table::Table(int philosophers, Logger& log)
  : philosophers_()
  , log_(log)
{
  for (int i = 0; i < philosophers; i++)
    philosophers_.emplace_back(std::make_shared<Philosopher>(i, log_));
}

Table::~Table()
{

}


std::size_t Table::get_minimum_drink_count() const
{
  if (philosophers_.empty())
    return 0;

  // Start with the first guy
  std::size_t min_drinks = philosophers_[0]->get_drink_count();

  // Look to see if anyone is lower
  for (auto const & philosopher : philosophers_)
  {
    auto count = philosopher->get_drink_count();

    if (count < min_drinks)
      min_drinks = count;
  }

  return min_drinks;
}
