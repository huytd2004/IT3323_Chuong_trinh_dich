/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __ERROR_H__ // Tránh include header file này nhiều lần
#define __ERROR_H__

// Danh sách các mã lỗi có thể xảy ra khi phân tích từ vựng / cú pháp
typedef enum
{
  ERR_ENDOFCOMMENT,        // Thiếu dấu kết thúc comment (ví dụ: thiếu */ )
  ERR_IDENTTOOLONG,        // Tên định danh quá dài vượt quá giới hạn cho phép
  ERR_INVALIDCHARCONSTANT, // Hằng ký tự không hợp lệ (ví dụ: 'ab', ký tự escape sai…)
  ERR_INVALIDSYMBOL        // Gặp ký hiệu không hợp lệ trong ngôn ngữ
} ErrorCode;

// Chuỗi mô tả tương ứng cho từng mã lỗi bên trên
#define ERM_ENDOFCOMMENT "End of comment expected!"   // Thông báo lỗi: thiếu kết thúc comment
#define ERM_IDENTTOOLONG "Identification too long!"   // Thông báo lỗi: tên định danh quá dài
#define ERM_INVALIDCHARCONSTANT "Invalid const char!" // Thông báo lỗi: hằng ký tự không hợp lệ
#define ERM_INVALIDSYMBOL "Invalid symbol!"           // Thông báo lỗi: ký hiệu không hợp lệ

// Hàm báo lỗi, nhận vào mã lỗi và vị trí lỗi (dòng + cột)
void error(ErrorCode err, int lineNo, int colNo);

#endif
