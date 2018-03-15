//////////////////////////////////////////////////////////////////////////
// IDrinkListener.h
//
// Copyright (C) 2018 Dan Sackinger - All Rights Reserved
// You may use, distribute and modify this code under the
// terms of the MIT license.
//
// IDrinkListener interface:
//  This is the interface where we will be notified when
//  a guest drinks
//

#if !defined(__IDRINKLISTENER_H__)
#define __IDRINKLISTENER_H__

#include <memory>

class IDrinkListener
{
public:
  IDrinkListener() = default;
  virtual ~IDrinkListener() = default;

public:
  // IDrinkListener interface
  virtual void report_drink(int id) = 0;
};

#endif // #if !defined(__IDRINKLISTENER_H__)

