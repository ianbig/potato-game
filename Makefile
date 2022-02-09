TARGETS = ringmaster player
CC = g++
CFLAG = --std=c++11 -Wall -Werror -Wextra

all: $(TARGETS)

ringmaster: potato ringmaster.cpp ringmaster.hpp
	$(CC) -o $@ $(CFLAG) -g potato.o ringmaster.cpp
player: potato player.cpp player.hpp
	$(CC) -o $@ $(CFLAG) $<
potato: potato.cpp potato.hpp
	$(CC) $< $(CFLAG) -c

.PHONY: clean
clean:
	rm -r -f *~ *.o $(TARGETS)
