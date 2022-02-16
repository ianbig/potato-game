TARGETS = ringmaster player
CXX = g++
NETWORKDEP = network.cpp network.hpp
RINGMASTER_HEADER = ringmaster.hpp $(NETWORKDEP)
RINGMASTER_SRC = ringmaster.cpp $(NETWORKDEP)
CXXFLAG = -pedantic --std=c++11 -Wall -Werror -Wextra
PLAYER_HEADER = player.hpp
PLAYER_SRC = player.cpp

all: $(TARGETS)

ringmaster: $(RINGMASTER_SRC) $(RINGMASTER_HEADER) $(NETWORKDEP)
	$(CXX) -o $@ $(CXXFLAG) -g ringmaster.cpp network.cpp
player: $(PLAYER_SRC) $(PLAYER_HEADER) $(NETWORKDEP)
	$(CXX) -o $@ $(CXXFLAG) -g player.cpp network.cpp

.PHONY: clean
clean:
	rm -r -f *~ *.o $(TARGETS)
