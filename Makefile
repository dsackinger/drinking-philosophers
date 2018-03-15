CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11
LDFLAGS=-g
LDLIBS=-lpthread

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
	$(RM) *.o
	$(RM) *~ .depend

include .depend

