CC = g++
CFLAGS = -Wall -std=c++17

build:
	$(CC) $(CFLAGS) MyBot.cpp -o MyBot

run:
	./MyBot

clean:
	rm -f MyBot
