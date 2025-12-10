#!/bin/bash
# filepath: test_all.sh

# Màu sắc cho output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Biên dịch chương trình
echo "Compiling..."
gcc -I. -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c symtab.c debug.c

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Compilation successful!${NC}\n"

# Khởi tạo biến đếm
total=0
passed=0
failed=0

# Kiểm tra từng test case
for i in {1..6}
do
    echo "=========================================="
    echo "Testing example${i}.kpl..."
    echo "=========================================="
    
    total=$((total + 1))
    
    # Chạy chương trình và lưu output
    ./main ../tests/example${i}.kpl > temp_output.txt 2>&1
    
    # Kiểm tra xem file result có tồn tại không
    if [ ! -f "../tests/result${i}.txt" ]; then
        echo -e "${YELLOW}Warning: result${i}.txt not found${NC}"
        echo "Actual output:"
        cat temp_output.txt
        echo ""
        continue
    fi
    
    # So sánh output với kết quả mong đợi
    diff -w temp_output.txt ../tests/result${i}.txt > /dev/null 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ PASSED${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}✗ FAILED${NC}"
        failed=$((failed + 1))
        echo ""
        echo "Expected output:"
        cat ../tests/result${i}.txt
        echo ""
        echo "Actual output:"
        cat temp_output.txt
        echo ""
        echo "Differences:"
        diff -u ../tests/result${i}.txt temp_output.txt
    fi
    echo ""
done

# Xóa file tạm
rm -f temp_output.txt

# Tổng kết
echo "=========================================="
echo "SUMMARY"
echo "=========================================="
echo -e "Total tests: ${total}"
echo -e "${GREEN}Passed: ${passed}${NC}"
echo -e "${RED}Failed: ${failed}${NC}"
echo "=========================================="

# Exit code
if [ $failed -eq 0 ]; then
    exit 0
else
    exit 1
fi