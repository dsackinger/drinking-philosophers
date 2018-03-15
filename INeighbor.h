// INeighbor interface
//
// This interface is implemented by a Philosopher and is used for one philosopher to
// communicate with the other
//

#pragma once

#include <memory>

class INeighbor
{
public:
  INeighbor() = default;
  virtual ~INeighbor() = default;

public:
  // INeighbor interface
  virtual int get_id() = 0;
  virtual void introduce_neighbor(std::shared_ptr<INeighbor> neighbor) = 0;
  virtual void send_bottle(int sender_id, bool dirty) = 0;
  virtual void send_request(int sender_id) = 0;
  virtual bool has_bottle(int id) = 0;
  virtual bool has_request(int id) = 0;
};