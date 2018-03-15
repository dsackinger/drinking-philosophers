CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)

SRCS = \
 main.cpp \
 Philosopher.cpp \
 Table.cpp \
 Logger.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

all: philo

philo: $(OBJS)
	$(CXX) $(LDFLAGS) -o philo $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>> ./.depend;

clean:
	$(RM) *~ .depend

include .depend

