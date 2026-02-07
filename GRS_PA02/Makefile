# Roll No: MT25056
# Makefile for PA02: Multithreaded Socket Implementations

CC = gcc
CFLAGS = -Wall -Wextra -lpthread
TARGETS = server_a1 client_a1 server_a2 client_a2 server_a3 client_a3

all: $(TARGETS)

# Baseline: Two-Copy
server_a1: MT25056_Part_A1_Server.c
	$(CC) MT25056_Part_A1_Server.c -o server_a1 $(CFLAGS)

client_a1: MT25056_Part_A1_Client.c
	$(CC) MT25056_Part_A1_Client.c -o client_a1 $(CFLAGS)

# One-Copy
server_a2: MT25056_Part_A2_Server.c
	$(CC) MT25056_Part_A2_Server.c -o server_a2 $(CFLAGS)

client_a2: MT25056_Part_A2_Client.c
	$(CC) MT25056_Part_A2_Client.c -o client_a2 $(CFLAGS)

# Zero-Copy
server_a3: MT25056_Part_A3_Server.c
	$(CC) MT25056_Part_A3_Server.c -o server_a3 $(CFLAGS)

client_a3: MT25056_Part_A3_Client.c
	$(CC) MT25056_Part_A3_Client.c -o client_a3 $(CFLAGS)

clean:
	rm -f $(TARGETS)