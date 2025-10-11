# SOAFEE Virtual ECU Makefile
CC = gcc
CFLAGS = -Wall -Wextra

# Directories
VECU_DIR = vecu

# Source files
MAIN_SRC = $(VECU_DIR)/main.c
SIM_SRC = $(VECU_DIR)/sensor_simulator.c

# Target executables
MAIN_TARGET = soafee_vecu
SIM_TARGET = sensor_simulator

# Default target
all: $(MAIN_TARGET) $(SIM_TARGET)

# Main vECU executable
$(MAIN_TARGET): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $(MAIN_TARGET) $(MAIN_SRC)

# Simulator executable
$(SIM_TARGET): $(SIM_SRC)
	$(CC) $(CFLAGS) -o $(SIM_TARGET) $(SIM_SRC)

# Docker container (same name as application)
docker-build:
	docker build -t $(MAIN_TARGET) .

# Install dependencies (Ubuntu/Debian)
deps:
	sudo apt-get update
	sudo apt-get install -y build-essential libjson-c-dev

# Clean build files
clean:
	rm -rf $(MAIN_TARGET)

# Clean and rebuild
rebuild: clean all

.PHONY: all clean deps docker-build rebuild
