#!/bin/bash

# Script to test the KPL compiler
# Compiles all .kpl files in tests/ directory and compares with expected output

COMPILER="./kplc"
TEST_DIR="./tests"
OUTPUT_DIR="./output"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${YELLOW}Compiler not found. Building...${NC}"
    make clean && make
    if [ ! -f "$COMPILER" ]; then
        echo -e "${RED}Failed to build compiler!${NC}"
        exit 1
    fi
fi

echo "=========================================="
echo "       KPL Compiler Test Suite"
echo "=========================================="
echo ""

PASSED=0
FAILED=0
TOTAL=0

# Find all .kpl files in tests directory
for kpl_file in "$TEST_DIR"/*.kpl; do
    if [ -f "$kpl_file" ]; then
        # Get the base name without extension
        base_name=$(basename "$kpl_file" .kpl)
        expected_file="$TEST_DIR/$base_name"
        output_file="$OUTPUT_DIR/$base_name"
        
        TOTAL=$((TOTAL + 1))
        
        echo -n "Testing $base_name.kpl ... "
        
        # Check if expected output file exists
        if [ ! -f "$expected_file" ]; then
            echo -e "${YELLOW}SKIPPED (no expected output)${NC}"
            continue
        fi
        
        # Compile the .kpl file
        $COMPILER "$kpl_file" "$output_file" 2>/dev/null
        
        if [ $? -ne 0 ]; then
            echo -e "${RED}FAILED (compilation error)${NC}"
            FAILED=$((FAILED + 1))
            continue
        fi
        
        # Compare output with expected
        if diff -q "$output_file" "$expected_file" > /dev/null 2>&1; then
            echo -e "${GREEN}PASSED${NC}"
            PASSED=$((PASSED + 1))
        else
            echo -e "${RED}FAILED (output mismatch)${NC}"
            echo "  Expected: $(xxd -l 32 "$expected_file" | head -1)"
            echo "  Got:      $(xxd -l 32 "$output_file" | head -1)"
            FAILED=$((FAILED + 1))
        fi
    fi
done

echo ""
echo "=========================================="
echo "               Summary"
echo "=========================================="
echo -e "Total: $TOTAL | ${GREEN}Passed: $PASSED${NC} | ${RED}Failed: $FAILED${NC}"
echo ""

# Test with kplrun if available
if command -v kplrun &> /dev/null; then
    echo "=========================================="
    echo "     Running tests with kplrun"
    echo "=========================================="
    for output_file in "$OUTPUT_DIR"/*; do
        if [ -f "$output_file" ]; then
            base_name=$(basename "$output_file")
            echo -n "Running $base_name ... "
            # Run with timeout to avoid infinite loops
            timeout 5s kplrun "$output_file" < /dev/null > /dev/null 2>&1
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}OK${NC}"
            else
                echo -e "${YELLOW}RUNTIME ERROR or TIMEOUT${NC}"
            fi
        fi
    done
else
    echo -e "${YELLOW}Note: kplrun not found in PATH. Skipping runtime tests.${NC}"
    echo "To run the generated code, use: kplrun <output_file>"
fi

exit $FAILED
