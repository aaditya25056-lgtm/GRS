# Makefile for GRS Assignment 1
# User: MT25056

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpthread

# --- FILENAMES FROM YOUR SCREENSHOT ---
# Note: Using 'Part_B_workers.c' as seen in your image
SRC_A = MT25056_Part_A_Program_A.c
SRC_B = MT25056_Part_A_Program_B.c
SRC_WORKER = MT25056_Part_B_workers.c
HEADER = MT25056_Part_B_workers.h

# Object files
OBJ_A = $(SRC_A:.c=.o) $(SRC_WORKER:.c=.o)
OBJ_B = $(SRC_B:.c=.o) $(SRC_WORKER:.c=.o)

# Executable names (Scripts expect simple names 'programA' and 'programB')
TARGET_A = programA
TARGET_B = programB

# --- BUILD RULES ---

all: $(TARGET_A) $(TARGET_B)

$(TARGET_A): $(OBJ_A)
	$(CC) $(CFLAGS) -o $(TARGET_A) $(OBJ_A)

$(TARGET_B): $(OBJ_B)
	$(CC) $(CFLAGS) -o $(TARGET_B) $(OBJ_B) $(LDFLAGS)

# Rule to compile .c to .o (Depends on your specific header)
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET_A) $(TARGET_B) *.png *.csv *.log *.txt *.log

# --- RUN COMMANDS (Using your specific shell script names) ---

run_partC: all
	chmod +x MT25056_Part_C_shell.sh
	./MT25056_Part_C_shell.sh

run_partD: all
	chmod +x MT25056_Part_D_shell.sh
	./MT25056_Part_D_shell.sh