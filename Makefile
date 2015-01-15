# Makefile for linking against ROOT 
# M. Marino 22 May 2007 

HARDWARE_CDT=/home/mgmarino/Dropbox/tmp/CDTWare/Software/HardwareLib_5.1
UNIVERSE=VMELinux/universe_api
UNIVERSEDRIVER=VMELinux/driver

TARGETS = main 
SOURCES = $(wildcard *.cc) #uncomment these to add all cc files in directory to your compile list 
SOURCES += $(wildcard *.C) #uncomment these to add all cc files in directory to your compile list 
OBJS = $(SOURCES:.cc=.o) 


TARGETOBJ = $(patsubst %, %.o, $(TARGETS))
CXX = g++
CXXFLAGS = -Wall -Wextra -g 
LIBS =  

CXXFLAGS += -I. -I$(HARDWARE_CDT)/src -I$(UNIVERSE) -I$(UNIVERSEDRIVER)
LIBS += -L$(HARDWARE_CDT) -L$(UNIVERSE) -lHardwareLib -luniverse_api

all: $(TARGETS)

$(TARGETS): $(TARGETOBJ) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< 


clean:
	rm -f $(TARGETS)
	rm -f *.o
