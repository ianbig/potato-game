TARGETS = ringmaster player
CXX = g++
RINGMASTER_HEADER = ringmaster.hpp network.hpp
RINGMASTER_SRC = ringmaster.cpp
CXXFLAG = -pedantic --std=c++11 -Wall -Werror -Wextra
PLAYER_HEADER = player.hpp network.hpp
PLAYER_SRC = player.cpp

all: $(TARGETS)

ringmaster: $(RINGMASTER_SRC) $(RINGMASTER_HEADER)
	$(CXX) -o $@ $(CXXFLAG) -g ringmaster.cpp
player: $(PLAYER_SRC) $(PLAYER_HEADER)
	$(CXX) -o $@ $(CXXFLAG) -g player.cpp

.PHONY: clean
clean:
	rm -r -f *~ *.o $(TARGETS)
