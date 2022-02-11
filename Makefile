TARGETS = ringmaster player
CC = g++
RINGMASTER_HEADER = ringmaster.hpp connect.hpp network.hpp
RINGMASTER_SRC = ringmaster.cpp
CFLAG = --std=c++11 -Wall -Werror -Wextra

all: $(TARGETS)

ringmaster: $(RINGMASTER_SRC) $(RINGMASTER_HEADER)
	$(CC) -o $@ $(CFLAG) -g ringmaster.cpp
player: potato player.cpp player.hpp
	$(CC) -o $@ $(CFLAG) -g player.cpp

.PHONY: clean
clean:
	rm -r -f *~ *.o $(TARGETS)
