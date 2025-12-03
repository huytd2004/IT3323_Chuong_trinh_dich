/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "token.h"

/**
 * Bảng tra cứu các từ khóa (keywords) của ngôn ngữ KPL
 * Mỗi phần tử gồm:
 *   - string: Tên keyword viết HOA (PROGRAM, CONST, ...)
 *   - tokenType: Loại token tương ứng (KW_PROGRAM, KW_CONST, ...)
 * Tổng cộng có 20 keywords được định nghĩa trong KEYWORDS_COUNT
 */
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

/**
 * So sánh 2 chuỗi không phân biệt HOA/thường
 * @param kw - Keyword chuẩn (đã viết HOA) từ bảng keywords
 * @param string - Chuỗi cần kiểm tra (có thể viết hoa/thường)
 * @return 1 nếu 2 chuỗi giống nhau (không phân biệt hoa/thường), 0 nếu khác
 *
 * Ví dụ:
 *   keywordEq("PROGRAM", "program") → 1
 *   keywordEq("PROGRAM", "Program") → 1
 *   keywordEq("PROGRAM", "BEGIN")   → 0
 */
int keywordEq(char *kw, char *string)
{
  while ((*kw != '\0') && (*string != '\0'))
  {
    if (*kw != toupper(*string))
      break;
    kw++;
    string++;
  }
  return ((*kw == '\0') && (*string == '\0'));
}

/**
 * Kiểm tra xem một chuỗi có phải là keyword hay không
 * @param string - Chuỗi cần kiểm tra (identifier đã đọc từ mã nguồn)
 * @return TokenType tương ứng nếu là keyword (KW_PROGRAM, KW_BEGIN, ...)
 *         TK_NONE nếu không phải keyword (là identifier thông thường)
 *
 * Cách hoạt động:
 *   - Duyệt qua toàn bộ 20 keywords trong bảng keywords[]
 *   - So sánh chuỗi với từng keyword bằng hàm keywordEq()
 *   - Trả về tokenType nếu tìm thấy, TK_NONE nếu không tìm thấy
 *
 * Ví dụ:
 *   checkKeyword("PROGRAM") → KW_PROGRAM
 *   checkKeyword("myVar")   → TK_NONE (là identifier)
 */
TokenType checkKeyword(char *string)
{
  int i;
  for (i = 0; i < KEYWORDS_COUNT; i++)
    if (keywordEq(keywords[i].string, string))
      return keywords[i].tokenType;
  return TK_NONE;
}

/**
 * Tạo một token mới và cấp phát bộ nhớ
 * @param tokenType - Loại token (KW_PROGRAM, TK_IDENT, SB_SEMICOLON, ...)
 * @param lineNo - Số dòng xuất hiện token trong mã nguồn (bắt đầu từ 1)
 * @param colNo - Số cột xuất hiện token trong mã nguồn (bắt đầu từ 1)
 * @return Con trỏ đến Token mới được tạo
 *
 * Lưu ý:
 *   - Bộ nhớ được cấp phát động bằng malloc()
 *   - Người gọi có trách nhiệm giải phóng bộ nhớ bằng free() sau khi dùng xong
 *   - Trường string và value chưa được khởi tạo, cần gán sau nếu cần
 *
 * Ví dụ:
 *   Token *t = makeToken(KW_PROGRAM, 1, 1);
 *   // ... sử dụng token ...
 *   free(t);
 */
Token *makeToken(TokenType tokenType, int lineNo, int colNo)
{
  Token *token = (Token *)malloc(sizeof(Token));
  token->tokenType = tokenType;
  token->lineNo = lineNo;
  token->colNo = colNo;
  return token;
}

/**
 * Chuyển đổi TokenType thành chuỗi mô tả để hiển thị
 * @param tokenType - Loại token cần chuyển đổi
 * @return Chuỗi mô tả token dưới dạng human-readable
 *
 * Sử dụng chính:
 *   - In thông báo lỗi khi thiếu token (Missing ...)
 *   - Hiển thị token trong quá trình debug
 *
 * Format trả về:
 *   - Keywords: "keyword PROGRAM", "keyword BEGIN", ...
 *   - Identifiers/Numbers: "an identification", "a number", ...
 *   - Symbols: "';'", "':='", "'('", ...
 *
 * Ví dụ:
 *   tokenToString(KW_PROGRAM)    → "keyword PROGRAM"
 *   tokenToString(SB_SEMICOLON)  → "';'"
 *   tokenToString(TK_IDENT)      → "an identification"
 */
char *tokenToString(TokenType tokenType)
{
  switch (tokenType)
  {
  case TK_NONE:
    return "None";
  case TK_IDENT:
    return "an identification";
  case TK_NUMBER:
    return "a number";
  case TK_CHAR:
    return "a constant char";
  case TK_EOF:
    return "end of file";

  case KW_PROGRAM:
    return "keyword PROGRAM";
  case KW_CONST:
    return "keyword CONST";
  case KW_TYPE:
    return "keyword TYPE";
  case KW_VAR:
    return "keyword VAR";
  case KW_INTEGER:
    return "keyword INTEGER";
  case KW_CHAR:
    return "keyword CHAR";
  case KW_ARRAY:
    return "keyword ARRAY";
  case KW_OF:
    return "keyword OF";
  case KW_FUNCTION:
    return "keyword FUNCTION";
  case KW_PROCEDURE:
    return "keyword PROCEDURE";
  case KW_BEGIN:
    return "keyword BEGIN";
  case KW_END:
    return "keyword END";
  case KW_CALL:
    return "keyword CALL";
  case KW_IF:
    return "keyword IF";
  case KW_THEN:
    return "keyword THEN";
  case KW_ELSE:
    return "keyword ELSE";
  case KW_WHILE:
    return "keyword WHILE";
  case KW_DO:
    return "keyword DO";
  case KW_FOR:
    return "keyword FOR";
  case KW_TO:
    return "keyword TO";

  case SB_SEMICOLON:
    return "\';\'";
  case SB_COLON:
    return "\':\'";
  case SB_PERIOD:
    return "\'.\'";
  case SB_COMMA:
    return "\',\'";
  case SB_ASSIGN:
    return "\':=\'";
  case SB_EQ:
    return "\'=\'";
  case SB_NEQ:
    return "\'!=\'";
  case SB_LT:
    return "\'<\'";
  case SB_LE:
    return "\'<=\'";
  case SB_GT:
    return "\'>\'";
  case SB_GE:
    return "\'>=\'";
  case SB_PLUS:
    return "\'+\'";
  case SB_MINUS:
    return "\'-\'";
  case SB_TIMES:
    return "\'*\'";
  case SB_SLASH:
    return "\'/\'";
  case SB_LPAR:
    return "\'(\'";
  case SB_RPAR:
    return "\')\'";
  case SB_LSEL:
    return "\'(.\'";
  case SB_RSEL:
    return "\'.)\'";
  default:
    return "";
  }
}
