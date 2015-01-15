# Makefile for linking against ROOT 
# M. Marino 22 May 2007 

HARDWARE_CDT=/home_nowrite/CDT\ Ware/Software/HardwareLib_5.1
UNIVERSE=/usr/local/universe
UNIVERSEDRIVER=VMELinux/driver

TARGETS = main main1
TARGETSCC = $(patsubst %, %.cc, $(TARGETS))
TARGETOBJ = $(patsubst %, %.o, $(TARGETS))
SOURCES = $(filter-out $(TARGETSCC), $(wildcard *.cc)) #uncomment these to add all cc files in directory to your compile list 
OBJS = $(SOURCES:.cc=.o) 


CXX = g++
CXXFLAGS = -Wall -Wextra -g 
LIBS =  

CXXFLAGS += -I. -I$(HARDWARE_CDT)/src -I$(UNIVERSE)/include#-I$(UNIVERSEDRIVER)
LIBS += -L$(HARDWARE_CDT) -L$(UNIVERSE)/lib -lHardwareLib -luniverse_api

.PHONY: clean

all: $(TARGETS)

$(TARGETS) : $(TARGETOBJ) $(OBJS)

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< 

%: %.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LIBS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
