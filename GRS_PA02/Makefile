# Roll No: MT25056
# Makefile for GRS PA02

CC = gcc
CFLAGS = -Wall -O2 -lpthread

# Targets
all: a1 a2 a3

# ---------------- A1 ----------------

a1:
	$(CC) MT25056_Part_A1_Server.c -o server_a1 $(CFLAGS)
	$(CC) MT25056_Part_A1_Client.c -o client_a1 $(CFLAGS)

# ---------------- A2 ----------------

a2:
	$(CC) MT25056_Part_A2_Server.c -o server_a2 $(CFLAGS)
	$(CC) MT25056_Part_A2_Client.c -o client_a2 $(CFLAGS)

# ---------------- A3 ----------------

a3:
	$(CC) MT25056_Part_A3_Server.c -o server_a3 $(CFLAGS)
	$(CC) MT25056_Part_A3_Client.c -o client_a3 $(CFLAGS)

# ---------------- Clean ----------------

clean:
	rm -f server_a1 client_a1
	rm -f server_a2 client_a2
	rm -f server_a3 client_a3
	rm -f *.png
	rm -f *.txt

