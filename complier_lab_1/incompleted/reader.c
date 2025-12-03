/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

// Code dùng để đọc ký tự từ tệp đầu vào (input file)
#include <stdio.h>
#include "reader.h"

// ---------------------------------------------------------
// Biến toàn cục
// ---------------------------------------------------------

FILE *inputStream; // Con trỏ tới file đang đọc
int lineNo, colNo; // Theo dõi vị trí hiện tại trong file (dòng và cột)
int currentChar;   // Ký tự hiện tại vừa đọc được

// ---------------------------------------------------------
// Hàm readChar: đọc 1 ký tự từ file và cập nhật vị trí
// ---------------------------------------------------------
int readChar(void)
{
  // Lấy 1 ký tự từ file (trả về EOF nếu hết file)
  currentChar = getc(inputStream);

  // Tăng số cột (mỗi ký tự là một cột)
  colNo++;

  // Nếu gặp ký tự xuống dòng, tăng dòng và reset cột
  if (currentChar == '\n')
  {
    lineNo++;
    colNo = 0;
  }

  return currentChar; // Trả về ký tự vừa đọc
}

// ---------------------------------------------------------
// Hàm openInputStream: mở file để chuẩn bị đọc
// ---------------------------------------------------------
int openInputStream(char *fileName)
{
  // Mở file ở chế độ text (rt = read text)
  inputStream = fopen(fileName, "rt");

  // Nếu mở thất bại → trả về mã lỗi IO_ERROR
  if (inputStream == NULL)
    return IO_ERROR;

  // Khởi tạo vị trí đọc ban đầu
  lineNo = 1;
  colNo = 0;

  // Đọc ký tự đầu tiên để chuẩn bị cho scanner
  readChar();

  return IO_SUCCESS; // Thành công
}

// ---------------------------------------------------------
// Hàm closeInputStream: đóng file sau khi đọc xong
// ---------------------------------------------------------
void closeInputStream()
{
  fclose(inputStream);
}
