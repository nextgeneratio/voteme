# VoteMe - Complete Voting System Makefile
# Comprehensive build system for all components

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -I./src
SRCDIR = src
OBJDIR = obj
BINDIR = bin
DATADIR = data

# Source files
MAIN_SOURCES = $(SRCDIR)/main.c $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/voting.c \
               $(SRCDIR)/voter_register.c $(SRCDIR)/candidate_register.c
ADMIN_SOURCES = $(SRCDIR)/admin.c $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/voting.c
VOTING_SOURCES = $(SRCDIR)/voting.c $(SRCDIR)/data_handler_enhanced.c

# Object files
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
ADMIN_OBJECTS = $(ADMIN_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
VOTING_OBJECTS = $(VOTING_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Executables
MAIN_TARGET = $(BINDIR)/voteme
ADMIN_TARGET = $(BINDIR)/admin
VOTING_TARGET = $(BINDIR)/voting_demo

# Colors for output
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m # No Color

.PHONY: all main admin voting clean setup help demo status test install

# Default target
all: setup main

# Main comprehensive application
main: setup $(MAIN_TARGET)
	@echo "$(GREEN)✅ VoteMe main application built successfully!$(NC)"
	@echo "$(BLUE)💡 Run with: ./$(MAIN_TARGET)$(NC)"

# Admin system only
admin: setup $(ADMIN_TARGET)
	@echo "$(GREEN)✅ Admin system built successfully!$(NC)"
	@echo "$(BLUE)💡 Run with: ./$(ADMIN_TARGET)$(NC)"

# Voting demo
voting: setup $(VOTING_TARGET)
	@echo "$(GREEN)✅ Voting demo built successfully!$(NC)"
	@echo "$(BLUE)💡 Run with: ./$(VOTING_TARGET)$(NC)"

# Setup directories and data files
setup:
	@echo "$(BLUE)🔧 Setting up build environment...$(NC)"
	@mkdir -p $(OBJDIR) $(BINDIR) $(DATADIR)
	@echo "$(GREEN)✅ Build environment ready$(NC)"

# Main application
$(MAIN_TARGET): $(MAIN_OBJECTS)
	@echo "$(BLUE)🔨 Linking main application...$(NC)"
	$(CC) $(CFLAGS) -o $@ $^

# Admin application
$(ADMIN_TARGET): $(ADMIN_OBJECTS)
	@echo "$(BLUE)🔨 Linking admin application...$(NC)"
	$(CC) $(CFLAGS) -o $@ $^

# Voting demo
$(VOTING_TARGET): $(VOTING_OBJECTS)
	@echo "$(BLUE)🔨 Linking voting demo...$(NC)"
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "$(BLUE)🔨 Compiling $<...$(NC)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create demo data
demo-data:
	@echo "$(BLUE)📄 Creating demo data...$(NC)"
	@echo "voting_number,name,nic,district_id" > $(DATADIR)/approved_voters.txt
	@echo "V001,John Doe,123456789V,D01" >> $(DATADIR)/approved_voters.txt
	@echo "V002,Jane Smith,987654321V,D01" >> $(DATADIR)/approved_voters.txt
	@echo "V003,Bob Johnson,456789123V,D02" >> $(DATADIR)/approved_voters.txt
	@echo "V004,Alice Brown,789123456V,D02" >> $(DATADIR)/approved_voters.txt
	@echo "V005,Charlie Wilson,321654987V,D01" >> $(DATADIR)/approved_voters.txt
	
	@echo "candidate_number,name,party_id,district_id,nic" > $(DATADIR)/approved_candidates.txt
	@echo "C001,Michael Lee,P001,D01,111222333V" >> $(DATADIR)/approved_candidates.txt
	@echo "C002,Sarah Davis,P002,D01,444555666V" >> $(DATADIR)/approved_candidates.txt
	@echo "C003,David Miller,P001,D02,777888999V" >> $(DATADIR)/approved_candidates.txt
	@echo "C004,Emma Wilson,P003,D02,000111222V" >> $(DATADIR)/approved_candidates.txt
	
	@echo "party_id,party_name" > $(DATADIR)/party_name.txt
	@echo "P001,Democratic Alliance" >> $(DATADIR)/party_name.txt
	@echo "P002,People's Progressive" >> $(DATADIR)/party_name.txt
	@echo "P003,National Unity" >> $(DATADIR)/party_name.txt
	
	@echo "district_id,district_name" > $(DATADIR)/district.txt
	@echo "D01,Metropolitan North" >> $(DATADIR)/district.txt
	@echo "D02,Metropolitan South" >> $(DATADIR)/district.txt
	
	@echo "voter_id,candidate_id" > $(DATADIR)/votes.txt
	@echo "V001,C001" >> $(DATADIR)/votes.txt
	@echo "V002,C002" >> $(DATADIR)/votes.txt
	@echo "V003,C001" >> $(DATADIR)/votes.txt
	@echo "V004,C003" >> $(DATADIR)/votes.txt
	@echo "V005,C002" >> $(DATADIR)/votes.txt
	
	@echo "$(GREEN)✅ Demo data created successfully!$(NC)"

# Run demo with sample data
demo: main demo-data
	@echo "$(YELLOW)🎬 Starting VoteMe Demo...$(NC)"
	@echo "$(BLUE)📊 Demo data has been created in $(DATADIR)/$(NC)"
	@echo "$(GREEN)🚀 Launching application...$(NC)"
	./$(MAIN_TARGET)

# Test build (compile without linking)
test:
	@echo "$(BLUE)🧪 Testing compilation...$(NC)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/main.c -o /tmp/test_main.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/data_handler_enhanced.c -o /tmp/test_data.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/voting.c -o /tmp/test_voting.o
	@rm -f /tmp/test_*.o
	@echo "$(GREEN)✅ All source files compile successfully!$(NC)"

# Show project status
status:
	@echo "$(BLUE)📊 VoteMe Project Status$(NC)"
	@echo "=================================="
	@echo "$(YELLOW)Build Targets:$(NC)"
	@if [ -f $(MAIN_TARGET) ]; then echo "  ✅ Main Application: $(MAIN_TARGET)"; else echo "  ❌ Main Application: Not built"; fi
	@if [ -f $(ADMIN_TARGET) ]; then echo "  ✅ Admin System: $(ADMIN_TARGET)"; else echo "  ❌ Admin System: Not built"; fi
	@if [ -f $(VOTING_TARGET) ]; then echo "  ✅ Voting Demo: $(VOTING_TARGET)"; else echo "  ❌ Voting Demo: Not built"; fi
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
	@echo "  make all          - Build main application (default)"
	@echo "  make main         - Build main VoteMe application"
	@echo "  make admin        - Build admin system only"
	@echo "  make voting       - Build voting demo"
	@echo ""
	@echo "$(YELLOW)Development:$(NC)"
	@echo "  make test         - Test compilation"
	@echo "  make demo         - Create demo data and run"
	@echo "  make demo-data    - Create sample data files"
	@echo "  make status       - Show project status"
	@echo ""
	@echo "$(YELLOW)Maintenance:$(NC)"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make clean-all    - Clean everything including data"
	@echo "  make install-deps - Install system dependencies"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "$(GREEN)💡 Quick Start: make demo$(NC)"

# Build dependencies
$(OBJDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/data_handler_enhanced.h $(SRCDIR)/voting.h
$(OBJDIR)/data_handler_enhanced.o: $(SRCDIR)/data_handler_enhanced.c $(SRCDIR)/data_handler_enhanced.h
$(OBJDIR)/voting.o: $(SRCDIR)/voting.c $(SRCDIR)/voting.h $(SRCDIR)/data_handler_enhanced.h
$(OBJDIR)/admin.o: $(SRCDIR)/admin.c $(SRCDIR)/data_handler_enhanced.h $(SRCDIR)/voting.h
$(OBJDIR)/voter_register.o: $(SRCDIR)/voter_register.c $(SRCDIR)/voter_register.h
$(OBJDIR)/candidate_register.o: $(SRCDIR)/candidate_register.c $(SRCDIR)/candidate_register.h