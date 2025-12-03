/*
 * reader.h - Module đọc ký tự từ file
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __READER_H__ // Include guard để tránh include nhiều lần
#define __READER_H__

// Kết quả I/O
#define IO_ERROR 0   // Thất bại khi mở/đọc file
#define IO_SUCCESS 1 // Thành công

// Hàm đọc ký tự tiếp theo từ file nguồn
int readChar(void);

// Mở file input và chuẩn bị cho việc đọc ký tự
// fileName: tên file nguồn
// Trả về IO_SUCCESS hoặc IO_ERROR
int openInputStream(char *fileName);

// Đóng file input khi hoàn thành
void closeInputStream(void);

#endif
