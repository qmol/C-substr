# Makefile for C-substr project
# A substring function parser and generator for different DBMS systems

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
LDFLAGS = -lm

# Project name and directories
PROJECT = c-substr
SRCDIR = .
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = main.c substr_wrapper.c
HEADERS = substr_wrapper.h func_status.h
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/$(PROJECT)

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Build the main executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build with additional debug flags
debug: CFLAGS += -DDEBUG -g3 -O0
debug: $(TARGET)

# Release build with optimizations
release: CFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Clean and rebuild
rebuild: clean all

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(PROJECT)

# Check for memory leaks with valgrind
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Static analysis with cppcheck
analyze:
	cppcheck --enable=all --std=c99 $(SOURCES)

# Format code with clang-format
format:
	clang-format -i $(SOURCES) $(HEADERS)

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  run       - Build and run the program"
	@echo "  debug     - Build with debug flags"
	@echo "  release   - Build optimized release version"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  memcheck  - Run with valgrind memory checker"
	@echo "  analyze   - Run static analysis with cppcheck"
	@echo "  format    - Format code with clang-format"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all run debug release clean rebuild install uninstall memcheck analyze format help

# Dependencies
$(OBJDIR)/main.o: main.c substr_wrapper.h func_status.h
$(OBJDIR)/substr_wrapper.o: substr_wrapper.c substr_wrapper.h func_status.h
