bin_PROGRAMS = cmdhistogram$(EXEEXT)
cmdhistogram_SOURCES = makeHistogram.cpp
CXX = g++
CP = cp
cmdhistogram_LDADD = -lstdc++ $(shell root-config --libs)

cmdhistogram_CXXFLAGS = $(shell root-config --cflags)
TARGETS = cmdhistogram$(EXEEXT)
#SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))
INSTALL_DIR = $(HOME)/.local

.PHONY: all clean

all: $(TARGETS)

install: $(TARGETS)
	ln -sn $(PWD)/../bin/$(TARGETS) $(INSTALL_DIR)/bin

uninstall:
	unlink $(INSTALL_DIR)/bin/$(TARGETS)

%.o: %.cpp
	$(CXX) $(cmdhistogram_CXXFLAGS) $(cmdhistogram_LDADD) -o $@ -c $<

cmdhistogram$(EXEEXT): makeHistogram.o
	$(CXX) $(cmdhistogram_CXXFLAGS) $(cmdhistogram_LDADD) -o $@ $^
	$(CP) $@ ../bin/

clean:
	$(RM) $(OBJECTS) $(TARGETS)

