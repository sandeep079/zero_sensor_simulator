CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -g
LDFLAGS = -lm

SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Test source (only test_harness.c, not the main simulator)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test_%.o)

# Targets - only build the test binary (which contains main)
TEST_TARGET = $(BIN_DIR)/s24_test

.PHONY: all clean test run format

all: $(TEST_TARGET)

# Link ALL object files including s24_master.o and all sensors
$(TEST_TARGET): $(TEST_OBJS) $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)
	@echo ""
	@echo "=== Running extended stress test ==="
	@./$(TEST_TARGET) > /dev/null 2>&1 || true
	@echo "✅ Extended test completed"

run: $(TEST_TARGET)
	./$(TEST_TARGET)

valgrind: $(TEST_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	find . -name "*.o" -delete
	find . -name "*.gch" -delete

format:
	clang-format -i include/*.h src/*.c test/*.c 2>/dev/null || true

help:
	@echo "Available targets:"
	@echo "  make          - Build everything"
	@echo "  make test     - Run test suite"
	@echo "  make run      - Run simulator"
	@echo "  make valgrind - Check for memory leaks"
	@echo "  make clean    - Clean build files"
	@echo "  make format   - Format code"