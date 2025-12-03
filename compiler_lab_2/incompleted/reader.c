/*
 * reader.c - Module xử lý đọc ký tự từ file nguồn
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"

// FILE pointer dùng để lưu trữ file nguồn đang đọc
FILE *inputStream;

// Biến lưu số dòng và cột hiện tại, phục vụ cho thông báo lỗi và token
int lineNo, colNo;

// Biến lưu ký tự hiện tại đang đọc
int currentChar;

// Hàm đọc ký tự tiếp theo từ file nguồn
int readChar(void)
{
  currentChar = getc(inputStream); // Đọc 1 ký tự từ file
  colNo++;                         // Tăng số cột lên 1
  if (currentChar == '\n')
  {            // Nếu gặp xuống dòng
    lineNo++;  // Tăng số dòng
    colNo = 0; // Reset cột về 0
  }
  return currentChar; // Trả về ký tự vừa đọc
}

// Mở file nguồn để bắt đầu đọc
int openInputStream(char *fileName)
{
  inputStream = fopen(fileName, "rt"); // Mở file ở chế độ text read
  if (inputStream == NULL)             // Kiểm tra mở file có thành công không
    return IO_ERROR;
  lineNo = 1;        // Khởi tạo số dòng
  colNo = 0;         // Khởi tạo số cột
  readChar();        // Đọc ký tự đầu tiên
  return IO_SUCCESS; // Trả về thành công
}

// Đóng file sau khi đọc xong
void closeInputStream()
{
  fclose(inputStream);
}
