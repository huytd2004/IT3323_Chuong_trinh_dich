/*
 * To compile, use the command:
 * gcc -I. -o main scanner.c reader.c charcode.c token.c error.c parser.c main.c
 *   → Compile chương trình chính, include thư mục hiện tại (.) để tìm header files.
 *
 * To compile and run an example with one command:
 * gcc -I. -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c && ./main ../test/example1.kpl
 *   → Sau khi build xong thì chạy ngay file example1.kpl
 *
 * To compile a unittest file, use the command:
 * gcc -I. -I/opt/homebrew/include -L/opt/homebrew/lib -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c -lcmocka
 *   → Dùng thêm CMocka để test unit.
 *
 * Some explanations:
 * -I. : Include thư mục hiện tại để tìm *.h
 * -I/opt/homebrew/include : Include header của CMocka
 * -L/opt/homebrew/lib : Thư viện cmocka (.a / .dylib)
 * -lcmocka : Link thư viện cmocka vào chương trình
 */

/******************************************************************/
/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "parser.h"

/******************************************************************/

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("parser: no input file.\n");
    return -1;
  }

  if (compile(argv[1]) == IO_ERROR)
  {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}
