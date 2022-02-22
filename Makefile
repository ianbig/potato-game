TARGETS = ringmaster player potato-test
CXX = g++
NETWORKDEP = network.cpp network.hpp
RINGMASTER_HEADER = ringmaster.hpp $(NETWORKDEP)
RINGMASTER_SRC = ringmaster.cpp $(NETWORKDEP)
CXXFLAG = -pedantic --std=c++11 -Wall -Werror -Wextra
PLAYER_HEADER = player.hpp
PLAYER_SRC = player.cpp
POTATO_SRC = potato.cpp
POTATO_HEADER = potato.hpp

all: $(TARGETS)

ringmaster: $(RINGMASTER_SRC) $(RINGMASTER_HEADER) $(NETWORKDEP) $(POTATO_SRC) $(POTATO_HEADER)
	$(CXX) -o $@ $(CXXFLAG) -g ringmaster.cpp network.cpp potato.cpp
player: $(PLAYER_SRC) $(PLAYER_HEADER) $(NETWORKDEP) $(POTATO_SRC) $(POTATO_HEADER)
	$(CXX) -o $@ $(CXXFLAG) -g player.cpp network.cpp potato.cpp
potato-test:
	$(CXX) -o potato-test $(CXXFLAG) -g potato.cpp potato-test.cpp

.PHONY: clean
clean:
	rm -r -f *~ *.o $(TARGETS)
