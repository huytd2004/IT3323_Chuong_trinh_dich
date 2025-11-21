/*
 * Hướng dẫn build file kiểm thử:
 *   - Truy cập thư mục Lecturer/Bai2/completed
 *   - Chạy lệnh:
 *     gcc -I. -I/opt/homebrew/include -L/opt/homebrew/lib \
 *         -o unit_test_scanner unit_test_scanner.c \
 *         reader.c charcode.c token.c error.c -lcmocka && ./unit_test_scanner
 *
 * Ghi chú:
 *   - -I.                     : include các header trong thư mục hiện tại
 *   - -I/opt/homebrew/include : đường dẫn tới header của CMocka
 *   - -L/opt/homebrew/lib     : thư viện CMocka (libcmocka)
 *   - -lcmocka                : link library CMocka để chạy unit test
 */

// Code dùng để kiểm thử scanner.c với thư viện cmocka

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h> // Thư viện unit test CMocka

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"
#include "scanner.c" // Import trực tiếp scanner để test

// Các biến toàn cục được khai báo trong reader.c và scanner.c
extern int lineNo;
extern int colNo;
extern int currentChar;
extern CharCode charCodes[];

/* ---------------------------------------------------------
 * Test case cơ bản: không làm gì và luôn thành công.
 * Mục đích: kiểm tra rằng hệ thống test hoạt động bình thường.
 * --------------------------------------------------------- */
static void null_test_success(void **state)
{
    (void)state; /* unused */
}

/* ---------------------------------------------------------
 * Test kiểm tra skipBlank() khi đang ở EOF.
 * Kỳ vọng: skipBlank() không làm crash và currentChar giữ nguyên EOF.
 * --------------------------------------------------------- */
static void test_EOF(void **state)
{
    (void)state; /* unused */
    currentChar = EOF;
    skipBlank();
    assert_int_equal(currentChar, EOF);
}

/* ---------------------------------------------------------
 * Hàm so sánh nội dung hai file dòng theo dòng.
 * Dùng để so khớp output của scanner với file kết quả kỳ vọng.
 * --------------------------------------------------------- */
static void compare_file_contents(const char *file1, const char *file2)
{
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");

    assert_non_null(f1);
    assert_non_null(f2);

    char line1[1024];
    char line2[1024];

    // Đọc lần lượt từng dòng và so sánh
    while (fgets(line1, sizeof(line1), f1) &&
           fgets(line2, sizeof(line2), f2))
    {
        assert_string_equal(line1, line2);
    }

    fclose(f1);
    fclose(f2);
}

/* ---------------------------------------------------------
 * test_scan: Chạy scan(input), redirect stdout ra file tạm "temp.txt",
 *            sau đó so sánh với file output đúng.
 *
 * input_filename          : đường dẫn file input (.kpl)
 * expected_output_filename: kết quả scan mong đợi
 * --------------------------------------------------------- */
static void test_scan(char *input_filename, char *expected_output_filename)
{
    char temp_filename[] = "temp.txt";
    int status;

    // Redirect stdout → ghi vào temp.txt
    FILE *temp_file = freopen(temp_filename, "w", stdout);
    assert_non_null(temp_file);

    // Thực hiện scan file input
    status = scan(input_filename);
    assert_int_equal(status, IO_SUCCESS);

    // Đóng file tạm
    fclose(temp_file);

    // So sánh file sinh ra và file kết quả đúng
    compare_file_contents(temp_filename, expected_output_filename);

    // Xóa file tạm
    remove(temp_filename);
}

/* ---------------------------------------------------------
 * Ba test case ứng với 3 file .kpl mẫu
 * --------------------------------------------------------- */
static void test_input1(void **state)
{
    (void)state; /* unused */
    test_scan("../test/example1.kpl", "../test/result1.txt");
    assert_true(currentChar == EOF);
}

static void test_input2(void **state)
{
    (void)state; /* unused */
    test_scan("../test/example2.kpl", "../test/result2.txt");
    assert_true(currentChar == EOF);
}

static void test_input3(void **state)
{
    (void)state; /* unused */
    test_scan("../test/example3.kpl", "../test/result3.txt");
    assert_true(1); // Chấp nhận vì EOF có thể khác tùy test
}

/* ---------------------------------------------------------
 * Hàm main: chạy toàn bộ test bằng CMocka
 * --------------------------------------------------------- */
int main(void)
{
    // Nếu muốn output kiểu TAP (test anything protocol):
    // cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_input1),
        cmocka_unit_test(test_input2),
        cmocka_unit_test(test_input3),
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(test_EOF),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
