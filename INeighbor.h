//////////////////////////////////////////////////////////////////////////
// INeighbor.h
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// INeighbor interface:
//  This interface is implemented by a Philosopher and is used for one
//  philosopher to communicate with the other.  In a real world solution,
//  this would be replaced with a communications layer
//

#if !defined(__INEIGHBOR_H__)
#define __INEIGHBOR_H__

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

#endif // #if !defined(__INEIGHBOR_H__)
