# ***************************************************************
# * File Name: Makefile
# * Description: Makefile for Post Office Simulation
# * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
# * Faculty: FIT VUT
# * Date: 28.04.2023
# * Refactored: August 2025
# *
# * Comments: Compiled using gcc 9.4.0+
# ***************************************************************

# Compiler settings
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -g
LDFLAGS = -pthread

# Project settings
TARGET = post_office_sim
OUTPUT_FILE = post_office.out
SRC_DIR = src
INCLUDE_DIR = include

# Source files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/args.c $(SRC_DIR)/utils.c $(SRC_DIR)/customer.c $(SRC_DIR)/worker.c
OBJECTS = $(SOURCES:.c=.o)

# Test parameters
NUM_RUNS := 1000
TEST_PARAMS = 3 2 100 100 100

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/post_office.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Run the simulation with test parameters
run: $(TARGET)
	./$(TARGET) $(TEST_PARAMS) && cat $(OUTPUT_FILE)

# Run multiple iterations for stress testing
stress_test: $(TARGET)
	@echo "Running $(NUM_RUNS) iterations of stress test..."
	@for i in $$(seq 1 $(NUM_RUNS)); do \
		echo "Iteration $$i/$(NUM_RUNS)"; \
		./$(TARGET) 39 12 73 6 258 > /dev/null 2>&1; \
		if [ $$? -ne 0 ]; then \
			echo "FAILED at iteration $$i"; \
			exit 1; \
		fi; \
	done
	@echo "All $(NUM_RUNS) iterations completed successfully!"

# Run with valgrind for memory leak detection
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		./$(TARGET) $(TEST_PARAMS)

# Run quick test suite
test: $(TARGET)
	@echo "Running basic functionality tests..."
	@echo "Test 1: Basic simulation"
	./$(TARGET) 5 2 50 50 100
	@echo "Test 2: No customers"
	./$(TARGET) 0 2 50 50 100
	@echo "Test 3: No workers (should handle gracefully)"
	./$(TARGET) 5 0 50 50 100
	@echo "Test 4: Quick closing"
	./$(TARGET) 10 3 200 50 10
	@echo "All tests completed!"

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) $(OUTPUT_FILE) *.tar.gz

# Show help
help:
	@echo "Post Office Simulation - Available targets:"
	@echo "  all          - Build the simulation (default)"
	@echo "  run          - Build and run with test parameters"
	@echo "  test         - Run test suite"
	@echo "  stress_test  - Run $(NUM_RUNS) iterations for stress testing"
	@echo "  valgrind     - Run with memory leak detection"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage: ./$(TARGET) NZ NU TZ TU F"
	@echo "  NZ: Number of customers"
	@echo "  NU: Number of office workers"
	@echo "  TZ: Max customer wait time (0-10000ms)"
	@echo "  TU: Max worker break time (0-100ms)"
	@echo "  F: Office open time (0-10000ms)"

.PHONY: all run test stress_test valgrind clean help
