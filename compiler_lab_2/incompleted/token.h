/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

// Độ dài tối đa của identifier (định danh)
#define MAX_IDENT_LEN 15

// Số lượng keywords trong ngôn ngữ KPL
#define KEYWORDS_COUNT 20

/**
 * Enum định nghĩa các loại token trong ngôn ngữ KPL
 *
 * Các nhóm token:
 * - TK_xxx: Token cơ bản (identifier, number, char, EOF)
 * - KW_xxx: Keywords (từ khóa) của ngôn ngữ
 * - SB_xxx: Symbols (ký hiệu đặc biệt, toán tử)
 */
typedef enum
{
  // Token cơ bản
  TK_NONE,   // Token không hợp lệ
  TK_IDENT,  // Identifier: tên biến, hàm, thủ tục (vd: myVar, count)
  TK_NUMBER, // Số nguyên (vd: 123, 456)
  TK_CHAR,   // Ký tự trong dấu nháy đơn (vd: 'a', 'Z')
  TK_EOF,    // End of file - kết thúc file

  // Keywords - Từ khóa khai báo
  KW_PROGRAM, // Từ khóa PROGRAM - bắt đầu chương trình
  KW_CONST,   // Từ khóa CONST - khai báo hằng số
  KW_TYPE,    // Từ khóa TYPE - khai báo kiểu dữ liệu, ví dụ struct, array
  KW_VAR,     // Từ khóa VAR - khai báo biến, ví dụ var x : Integer;

  // Keywords - Kiểu dữ liệu cơ bản
  KW_INTEGER, // Kiểu số nguyên
  KW_CHAR,    // Kiểu ký tự
  KW_ARRAY,   // Kiểu mảng
  KW_OF,      // Từ khóa OF - dùng trong khai báo mảng (ARRAY [10] OF INTEGER)

  // Keywords - Khai báo hàm/thủ tục
  KW_FUNCTION,  // Từ khóa FUNCTION - khai báo hàm (có giá trị trả về)
  KW_PROCEDURE, // Từ khóa PROCEDURE - khai báo thủ tục (không trả về giá trị)

  // Keywords - Cấu trúc khối lệnh
  KW_BEGIN, // Bắt đầu khối lệnh
  KW_END,   // Kết thúc khối lệnh
  KW_CALL,  // Gọi thủ tục/hàm

  // Keywords - Câu lệnh điều kiện
  KW_IF,   // Câu lệnh IF
  KW_THEN, // Câu lệnh THEN (sau IF)
  KW_ELSE, // Câu lệnh ELSE

  // Keywords - Vòng lặp
  KW_WHILE, // Vòng lặp WHILE
  KW_DO,    // Từ khóa DO (dùng trong WHILE và FOR)
  KW_FOR,   // Vòng lặp FOR
  KW_TO,    // Từ khóa TO (dùng trong FOR: FOR i := 1 TO 10)

  // Symbols - Dấu phân cách
  SB_SEMICOLON, // Dấu chấm phẩy ';' - phân cách câu lệnh
  SB_COLON,     // Dấu hai chấm ':' - khai báo kiểu (n : Integer)
  SB_PERIOD,    // Dấu chấm '.' - kết thúc chương trình
  SB_COMMA,     // Dấu phẩy ',' - phân cách tham số, đối số

  // Symbols - Toán tử so sánh và gán
  SB_ASSIGN, // Toán tử gán ':=' (vd: x := 5)
  SB_EQ,     // Toán tử bằng '=' (vd: if x = 5)
  SB_NEQ,    // Toán tử khác '!=' (vd: if x != 5)
  SB_LT,     // Toán tử nhỏ hơn '<'
  SB_LE,     // Toán tử nhỏ hơn hoặc bằng '<='
  SB_GT,     // Toán tử lớn hơn '>'
  SB_GE,     // Toán tử lớn hơn hoặc bằng '>='

  // Symbols - Toán tử số học
  SB_PLUS,  // Toán tử cộng '+'
  SB_MINUS, // Toán tử trừ '-'
  SB_TIMES, // Toán tử nhân '*'
  SB_SLASH, // Toán tử chia '/'

  // Symbols - Dấu ngoặc
  SB_LPAR, // Dấu ngoặc tròn trái '(' - dùng cho tham số, biểu thức
  SB_RPAR, // Dấu ngoặc tròn phải ')'
  SB_LSEL, // Dấu ngoặc vuông trái '[' - array selector (chỉ số mảng)
  SB_RSEL  // Dấu ngoặc vuông phải ']'
} TokenType;

/**
 * Cấu trúc Token - đại diện cho một token trong mã nguồn
 *
 * Ví dụ: với dòng code "Var count : Integer;"
 * - Token 1: {string:"Var", lineNo:1, colNo:1, tokenType:KW_VAR, value:0}
 * - Token 2: {string:"count", lineNo:1, colNo:5, tokenType:TK_IDENT, value:0}
 * - Token 3: {string:":", lineNo:1, colNo:11, tokenType:SB_COLON, value:0}
 * - Token 4: {string:"Integer", lineNo:1, colNo:13, tokenType:KW_INTEGER, value:0}
 * - Token 5: {string:";", lineNo:1, colNo:20, tokenType:SB_SEMICOLON, value:0}
 */
typedef struct
{
  char string[MAX_IDENT_LEN + 1]; // Chuỗi ký tự của token (tối đa 15 ký tự + '\0')
  int lineNo;                     // Số dòng xuất hiện token trong file
  int colNo;                      // Số cột xuất hiện token trong file
  TokenType tokenType;            // Loại token (KW_xxx, TK_xxx, SB_xxx)
  int value;                      // Giá trị số (chỉ dùng cho TK_NUMBER)
} Token;

/**
 * Kiểm tra xem một chuỗi có phải là keyword không
 *
 * @param string Chuỗi cần kiểm tra (vd: "PROGRAM", "BEGIN", "myVar")
 * @return TokenType tương ứng nếu là keyword (vd: KW_PROGRAM, KW_BEGIN)
 *         TK_NONE nếu không phải keyword (là identifier)
 *
 * Ví dụ:
 *   checkKeyword("PROGRAM") → trả về KW_PROGRAM
 *   checkKeyword("Begin")   → trả về KW_BEGIN (không phân biệt HOA/thường)
 *   checkKeyword("myVar")   → trả về TK_NONE (không phải keyword)
 */
TokenType checkKeyword(char *string);

/**
 * Tạo một token mới và cấp phát bộ nhớ
 *
 * @param tokenType Loại token (KW_xxx, TK_xxx, SB_xxx)
 * @param lineNo Số dòng của token
 * @param colNo Số cột của token
 * @return Con trỏ trỏ tới Token mới được tạo (cần free() sau khi dùng)
 *
 * Ví dụ:
 *   Token *t = makeToken(KW_PROGRAM, 1, 1);
 *   // t->tokenType = KW_PROGRAM
 *   // t->lineNo = 1
 *   // t->colNo = 1
 *   // t->string = "" (chưa được gán)
 *   // Nhớ gọi free(t) sau khi dùng xong
 */
Token *makeToken(TokenType tokenType, int lineNo, int colNo);

/**
 * Chuyển TokenType thành chuỗi mô tả để in ra
 *
 * @param tokenType Loại token cần chuyển đổi
 * @return Chuỗi mô tả token (dùng để in output)
 *
 * Ví dụ:
 *   tokenToString(KW_PROGRAM)    → "KW_PROGRAM"
 *   tokenToString(TK_IDENT)      → "TK_IDENT"
 *   tokenToString(SB_SEMICOLON)  → "SB_SEMICOLON"
 *   tokenToString(SB_PLUS)       → "SB_PLUS"
 *
 * Kết quả được dùng trong hàm printToken() để in output như:
 *   1-1:KW_PROGRAM
 *   1-9:TK_IDENT(demo)
 *   1-13:SB_SEMICOLON
 */
char *tokenToString(TokenType tokenType);

#endif
