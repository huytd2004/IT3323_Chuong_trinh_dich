/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
// Code dùng để định nghĩa token và các hàm liên quan đến token

#include <stdlib.h> // malloc()
#include <ctype.h>  // toupper()
#include "token.h"  // Định nghĩa Token, TokenType, hằng số, struct,...

// ----------------------------------------------------------------------
// Danh sách các từ khóa (keywords) của ngôn ngữ KPL
// Mỗi phần tử gồm:
//   - string      : tên keyword viết hoa (PROGRAM, CONST, ...)
//   - tokenType   : loại token tương ứng
// KEYWORDS_COUNT được định nghĩa trong token.h
// MAX_IDENT_LEN giúp kiểm soát độ dài định danh
// ----------------------------------------------------------------------

struct
{
  char string[MAX_IDENT_LEN + 1];
  TokenType tokenType;
} keywords[KEYWORDS_COUNT] = {
    {"PROGRAM", KW_PROGRAM},
    {"CONST", KW_CONST},
    {"TYPE", KW_TYPE},
    {"VAR", KW_VAR},
    {"INTEGER", KW_INTEGER},
    {"CHAR", KW_CHAR},
    {"ARRAY", KW_ARRAY},
    {"OF", KW_OF},
    {"FUNCTION", KW_FUNCTION},
    {"PROCEDURE", KW_PROCEDURE},
    {"BEGIN", KW_BEGIN},
    {"END", KW_END},
    {"CALL", KW_CALL},
    {"IF", KW_IF},
    {"THEN", KW_THEN},
    {"ELSE", KW_ELSE},
    {"WHILE", KW_WHILE},
    {"DO", KW_DO},
    {"FOR", KW_FOR},
    {"TO", KW_TO}};

// ----------------------------------------------------------------------
// keywordEq: so sánh chuỗi string với keyword
//   - kw     : keyword trong bảng từ khóa (đã in hoa)
//   - string : định danh đọc được từ input (có thể viết thường/hoa)
// Hàm so khớp theo từng ký tự nhưng chuyển string sang uppercase.
// Trả về 1 nếu bằng nhau hoàn toàn, 0 nếu khác.
// ----------------------------------------------------------------------
int keywordEq(char *kw, char *string)
{
  while ((*kw != '\0') && (*string != '\0'))
  {
    if (*kw != toupper(*string))
      break; // so sánh ký tự (string chuyển sang viết hoa)
    kw++;
    string++;
  }

  // Nếu cả hai chuỗi đều kết thúc → trùng khớp hoàn toàn
  return ((*kw == '\0') && (*string == '\0'));
}

// ----------------------------------------------------------------------
// checkKeyword: kiểm tra xem một chuỗi có phải là keyword hay không
// Nếu đúng → trả về TokenType tương ứng
// Nếu không → trả về TK_NONE (tức là định danh thông thường)
// ----------------------------------------------------------------------
TokenType checkKeyword(char *string)
{
  int i;

  // Duyệt tuần tự qua toàn bộ danh sách keywords
  for (i = 0; i < KEYWORDS_COUNT; i++)
    if (keywordEq(keywords[i].string, string))
      return keywords[i].tokenType;

  return TK_NONE; // Không phải keyword
}

// ----------------------------------------------------------------------
// makeToken: tạo một token mới
//   - tokenType : loại token (KW_xxx, SB_xxx, TK_IDENT,...)
//   - lineNo    : dòng xảy ra token
//   - colNo     : cột bắt đầu token
// Hàm cấp phát động 1 token và trả về con trỏ đến token đó.
// ----------------------------------------------------------------------
Token *makeToken(TokenType tokenType, int lineNo, int colNo)
{
  Token *token = (Token *)malloc(sizeof(Token)); // cấp phát bộ nhớ

  token->tokenType = tokenType; // gán loại token
  token->lineNo = lineNo;       // vị trí dòng
  token->colNo = colNo;         // vị trí cột

  return token;
}
