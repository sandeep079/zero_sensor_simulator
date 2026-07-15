#!/bin/bash

# S24 Zero Sensor Simulator - Run Script
# Usage: ./run.sh [command]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default command
COMMAND=${1:-help}

# Project root
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY="$PROJECT_ROOT/bin/s24_test"

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_header() {
    echo ""
    echo -e "${BLUE}===========================================${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}===========================================${NC}"
    echo ""
}

# Show help
show_help() {
    echo "S24 Zero Sensor Simulator - Run Script"
    echo ""
    echo "Usage: ./run.sh [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  build       - Build the project (clean + compile)"
    echo "  test        - Run all tests"
    echo "  run         - Run the simulator (same as test)"
    echo "  valgrind    - Run memory leak check"
    echo "  clean       - Clean build files"
    echo "  all         - Clean, build, test, and valgrind (full verification)"
    echo "  ci          - Simulate CI pipeline locally"
    echo "  logs        - Show log files"
    echo "  help        - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./run.sh build"
    echo "  ./run.sh test"
    echo "  ./run.sh all"
    echo "  ./run.sh ci"
    echo ""
}

# Build the project
build_project() {
    print_header "Building S24 Zero Sensor Simulator"
    
    if [ -f "$PROJECT_ROOT/Makefile" ]; then
        print_info "Running make..."
        make -C "$PROJECT_ROOT" clean
        make -C "$PROJECT_ROOT"
        print_success "Build complete!"
    else
        print_error "Makefile not found in $PROJECT_ROOT"
        exit 1
    fi
}

# Run tests
run_tests() {
    print_header "Running Tests"
    
    if [ -f "$BINARY" ]; then
        print_info "Running test suite..."
        "$BINARY"
        
        if [ $? -eq 0 ]; then
            print_success "All tests passed!"
        else
            print_error "Some tests failed!"
            exit 1
        fi
    else
        print_error "Binary not found at $BINARY"
        print_info "Please run: ./run.sh build"
        exit 1
    fi
}

# Run memory leak check
run_valgrind() {
    print_header "Memory Leak Check"
    
    if ! command -v valgrind &> /dev/null; then
        print_warning "Valgrind not installed. Installing..."
        sudo apt-get update && sudo apt-get install -y valgrind
    fi
    
    if [ -f "$BINARY" ]; then
        print_info "Running valgrind memory check..."
        valgrind --leak-check=full \
                 --show-leak-kinds=all \
                 --error-exitcode=1 \
                 --log-file=valgrind-out.txt \
                 "$BINARY" > /dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            print_success "No memory leaks detected!"
            rm -f valgrind-out.txt
        else
            print_warning "Possible memory issues found. Check valgrind-out.txt"
            cat valgrind-out.txt
        fi
    else
        print_error "Binary not found at $BINARY"
        print_info "Please run: ./run.sh build"
        exit 1
    fi
}

# Clean build files
clean_project() {
    print_header "Cleaning Build Files"
    
    if [ -f "$PROJECT_ROOT/Makefile" ]; then
        make -C "$PROJECT_ROOT" clean
        print_success "Clean complete!"
    else
        print_error "Makefile not found in $PROJECT_ROOT"
        exit 1
    fi
}

# Full CI simulation
run_ci() {
    print_header "Running CI Pipeline Locally"
    
    print_info "Step 1: Clean and Build"
    build_project
    
    print_info "Step 2: Run Tests"
    run_tests
    
    print_info "Step 3: Memory Check"
    run_valgrind
    
    print_success "CI pipeline simulation complete! All steps passed."
}

# Show logs
show_logs() {
    print_header "Recent Logs"
    
    LOG_FILES=$(find "$PROJECT_ROOT" -name "*.log" -type f 2>/dev/null)
    
    if [ -n "$LOG_FILES" ]; then
        for log in $LOG_FILES; do
            echo -e "${BLUE}--- $log ---${NC}"
            tail -20 "$log"
            echo ""
        done
    else
        print_info "No log files found."
        print_info "Run './run.sh test' to generate logs."
    fi
}

# Run everything
run_all() {
    clean_project
    build_project
    run_tests
    run_valgrind
    print_success "Full verification complete!"
}

# Check if binary exists and is executable
check_binary() {
    if [ ! -f "$BINARY" ]; then
        print_warning "Binary not found. Building first..."
        build_project
    fi
    
    if [ ! -x "$BINARY" ]; then
        chmod +x "$BINARY"
    fi
}

# Main command handling
case "$COMMAND" in
    build)
        build_project
        ;;
    test)
        check_binary
        run_tests
        ;;
    run)
        check_binary
        run_tests
        ;;
    valgrind)
        check_binary
        run_valgrind
        ;;
    clean)
        clean_project
        ;;
    all)
        run_all
        ;;
    ci)
        run_ci
        ;;
    logs)
        show_logs
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        print_error "Unknown command: $COMMAND"
        echo ""
        show_help
        exit 1
        ;;
esac

exit 0
