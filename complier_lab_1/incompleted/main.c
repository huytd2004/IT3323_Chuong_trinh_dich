/*
 * To compile, use the command:
 * gcc -I. -o main scanner.c reader.c charcode.c token.c error.c
 *   → Compile chương trình chính, include thư mục hiện tại (.) để tìm header files.
 *
 * To compile and run an example with one command:
 * gcc -I. -o main main.c reader.c charcode.c token.c error.c && ./main ../test/example1.kpl
 *   → Sau khi build xong thì chạy ngay file example1.kpl
 *
 * To compile a unittest file, use the command:
 * gcc -I. -I/opt/homebrew/include -L/opt/homebrew/lib -o main main.c reader.c charcode.c token.c error.c -lcmocka
 *   → Dùng thêm CMocka để test unit.
 *
 * Some explanations:
 * -I. : Include thư mục hiện tại để tìm *.h
 * -I/opt/homebrew/include : Include header của CMocka
 * -L/opt/homebrew/lib : Thư viện cmocka (.a / .dylib)
 * -lcmocka : Link thư viện cmocka vào chương trình
 */

/******************************************************************/

// #include "main.h"
// Không dùng vì code này chỉ là chương trình chính để chạy scanner

#include <stdio.h>  // Hàm nhập xuất cơ bản: printf(...)
#include <stdlib.h> // Thư viện tiêu chuẩn: exit, malloc, ...

#include "scanner.c" // Import trực tiếp file .c (không khuyến khích)
// Lý do: project này theo phong cách “small assignment”, không dùng header riêng
// → scanner.c chứa hàm scan() dùng để phân tích mã nguồn

// -------------------------------------------------------------
// Hàm main: điểm bắt đầu của chương trình
// -------------------------------------------------------------
int main(int argc, char *argv[])
{

  // Kiểm tra xem người dùng có nhập file đầu vào không
  if (argc <= 1)
  {
    printf("scanner: no input file.\n");
    return -1; // Trả về mã lỗi
  }

  // Gọi hàm scan từ scanner.c để phân tích file
  // Nếu scan() trả IO_ERROR → không đọc được file
  if (scan(argv[1]) == IO_ERROR)
  {
    printf("Can't read input file!\n");
    return -1;
  }

  // Chương trình kết thúc bình thường
  return 0;
}
