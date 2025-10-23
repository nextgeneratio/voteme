# VoteMe - Minimal Build System (Admin + Core + Tests)
# Simplified to keep only: data handling (enhanced), admin console, voting algorithm

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -I./src
SRCDIR = src
OBJDIR = obj
BINDIR = bin
DATADIR = data

# Source files (kept minimal)
ADMIN_SOURCES = $(SRCDIR)/admin.c $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/voting.c $(SRCDIR)/ui_utils.c

# Object files
ADMIN_OBJECTS = $(ADMIN_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Executables
ADMIN_TARGET = $(BINDIR)/admin
TEST_VOTING_TARGET = $(BINDIR)/test_voting
TEST_DATA_SMOKE_TARGET = $(BINDIR)/test_data_smoke

# Colors for output
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m # No Color

.PHONY: all admin clean setup help status test tests install

# Default target builds admin
all: setup admin

# Admin system only
admin: setup $(ADMIN_TARGET)
	@echo "$(GREEN)✅ Admin system built successfully!$(NC)"
	@echo "$(BLUE)💡 Run with: ./$(ADMIN_TARGET)$(NC)"

# Unit tests
tests: setup $(TEST_VOTING_TARGET) $(TEST_DATA_SMOKE_TARGET)
	@echo "$(GREEN)✅ Tests built successfully!$(NC)"
	@echo "$(BLUE)💡 Run with: make test$(NC)"

# Setup directories and data files
setup:
	@echo "$(BLUE)🔧 Setting up build environment...$(NC)"
	@mkdir -p $(OBJDIR) $(BINDIR) $(DATADIR)
	@echo "$(GREEN)✅ Build environment ready$(NC)"

# Admin application
$(ADMIN_TARGET): $(ADMIN_OBJECTS)
	@echo "$(BLUE)🔨 Linking admin application...$(NC)"
	$(CC) $(CFLAGS) -o $@ $^

# Test binaries
$(TEST_VOTING_TARGET): tests/test_voting.c $(SRCDIR)/voting.c $(SRCDIR)/voting.h $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/data_handler_enhanced.h
	@echo "$(BLUE)🔨 Building test_voting...$(NC)"
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_voting.c $(SRCDIR)/voting.c $(SRCDIR)/data_handler_enhanced.c -o $@

$(TEST_DATA_SMOKE_TARGET): tests/test_data_smoke.c $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/data_handler_enhanced.h
	@echo "$(BLUE)🔨 Building test_data_smoke...$(NC)"
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_data_smoke.c $(SRCDIR)/data_handler_enhanced.c -o $@

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "$(BLUE)🔨 Compiling $<...$(NC)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

## Demo data removed in minimal build. Use tests or admin to create data.

# Run tests
test: tests
	@echo "$(YELLOW)🧪 Running tests...$(NC)"
	@./$(TEST_DATA_SMOKE_TARGET)
	@./$(TEST_VOTING_TARGET)

# Show project status
status:
	@echo "$(BLUE)📊 VoteMe Project Status$(NC)"
	@echo "=================================="
	@echo "$(YELLOW)Build Targets:$(NC)"
	@if [ -f $(ADMIN_TARGET) ]; then echo "  ✅ Admin System: $(ADMIN_TARGET)"; else echo "  ❌ Admin System: Not built"; fi
	@if [ -f $(TEST_VOTING_TARGET) ]; then echo "  ✅ Test: $(TEST_VOTING_TARGET)"; else echo "  ❌ Test: test_voting not built"; fi
	@if [ -f $(TEST_DATA_SMOKE_TARGET) ]; then echo "  ✅ Test: $(TEST_DATA_SMOKE_TARGET)"; else echo "  ❌ Test: test_data_smoke not built"; fi
	@echo ""
	@echo "$(YELLOW)Data Files:$(NC)"
	@if [ -f $(DATADIR)/approved_voters.txt ]; then echo "  ✅ Voters: $$(wc -l < $(DATADIR)/approved_voters.txt) records"; else echo "  ❌ Voters: No data"; fi
	@if [ -f $(DATADIR)/approved_candidates.txt ]; then echo "  ✅ Candidates: $$(wc -l < $(DATADIR)/approved_candidates.txt) records"; else echo "  ❌ Candidates: No data"; fi
	@if [ -f $(DATADIR)/votes.txt ]; then echo "  ✅ Votes: $$(wc -l < $(DATADIR)/votes.txt) records"; else echo "  ❌ Votes: No data"; fi
	@if [ -f $(DATADIR)/voting_results.txt ]; then echo "  ✅ Results: Available"; else echo "  ❌ Results: Not generated"; fi

# Install system dependencies (Ubuntu/Debian)
install-deps:
	@echo "$(BLUE)📦 Installing system dependencies...$(NC)"
	sudo apt-get update
	sudo apt-get install -y build-essential gcc make
	@echo "$(GREEN)✅ Dependencies installed successfully!$(NC)"

# Clean build artifacts
clean:
	@echo "$(BLUE)🧹 Cleaning build artifacts...$(NC)"
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "$(GREEN)✅ Build artifacts cleaned$(NC)"

# Clean everything including data
clean-all: clean
	@echo "$(BLUE)🧹 Cleaning all data...$(NC)"
	rm -rf $(DATADIR)
	@echo "$(GREEN)✅ All files cleaned$(NC)"

# Show help
help:
	@echo "$(BLUE)🎯 VoteMe Build System Help$(NC)"
	@echo "=============================="
	@echo ""
	@echo "$(YELLOW)Main Targets:$(NC)"
	@echo "  make all          - Build admin (default)"
	@echo "  make admin        - Build admin system only"
	@echo ""
	@echo "$(YELLOW)Development:$(NC)"
	@echo "  make tests        - Build unit tests"
	@echo "  make test         - Build and run tests"
	@echo "  make status       - Show project status"
	@echo ""
	@echo "$(YELLOW)Maintenance:$(NC)"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make clean-all    - Clean everything including data"
	@echo "  make install-deps - Install system dependencies"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "$(GREEN)💡 Quick Start: make demo$(NC)"

## Build dependencies
$(OBJDIR)/data_handler_enhanced.o: $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/data_handler_enhanced.h
$(OBJDIR)/voting.o: $(SRCDIR)/voting.c $(SRCDIR)/voting.h $(SRCDIR)/data_handler_enhanced.h
$(OBJDIR)/admin.o: $(SRCDIR)/admin.c $(SRCDIR)/data_handler_enhanced.h $(SRCDIR)/voting.h
## Removed legacy modules: voter_register, candidate_register, main, display