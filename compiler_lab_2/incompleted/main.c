/*
 * Hướng dẫn biên dịch và chạy chương trình:
 *
 * 1. Biên dịch chương trình chính:
 * gcc -I. -o main scanner.c reader.c charcode.c token.c error.c parser.c main.c
 *   → Dùng -I. để include thư mục hiện tại (.) tìm các header file (*.h)
 *
 * 2. Biên dịch và chạy trực tiếp với một file example:
 * gcc -I. -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c && ./main ../test/example1.kpl
 *   → Sau khi build xong, chạy file example1.kpl
 *
 * 3. Biên dịch unit test với CMocka:
 * gcc -I. -I/opt/homebrew/include -L/opt/homebrew/lib -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c -lcmocka
 *   → -I/opt/homebrew/include : include header của CMocka
 *   → -L/opt/homebrew/lib : đường dẫn tới thư viện CMocka
 *   → -lcmocka : link thư viện CMocka vào chương trình
 */

/******************************************************************/
/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h" // Chứa hàm readSourceFile() và các chức năng đọc file
#include "parser.h" // Chứa hàm compile() và các chức năng phân tích cú pháp

/******************************************************************/

int main(int argc, char *argv[])
{
  // Kiểm tra tham số dòng lệnh
  if (argc <= 1)
  {
    printf("parser: no input file.\n"); // Nếu không có file input, thông báo lỗi
    return -1;
  }

  // Gọi hàm compile() từ parser để phân tích file input
  // compile() trả về IO_ERROR nếu không đọc được file
  if (compile(argv[1]) == IO_ERROR)
  {
    printf("Can\'t read input file!\n"); // Thông báo lỗi nếu file không tồn tại hoặc không mở được
    return -1;
  }

  // Nếu compile thành công, kết thúc chương trình trả về 0
  return 0;
}
