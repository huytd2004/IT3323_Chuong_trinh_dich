/*
 * Hướng dẫn biên dịch và chạy chương trình:
 * 2. Biên dịch và chạy trực tiếp với một file example:
 * gcc -I. -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c symtab.c debug.c && ./main ../tests/example1.kpl
 *   → Sau khi build xong, chạy file example1.kpl
 *
 * 3. Biên dịch unit test với CMocka:
 * gcc -I. -I/opt/homebrew/include -L/opt/homebrew/lib -o main main.c reader.c charcode.c token.c error.c parser.c scanner.c -lcmocka
 *   → -I/opt/homebrew/include : include header của CMocka
 *   → -L/opt/homebrew/lib : đường dẫn tới thư viện CMocka
 *   → -lcmocka : link thư viện CMocka vào chương trình
 */
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
