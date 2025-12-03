/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
// Code dùng để quét mã nguồn và tạo token
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

/**
 * Bỏ qua các ký tự khoảng trắng (space, tab, newline)
 * Đọc liên tục cho đến khi gặp ký tự không phải khoảng trắng hoặc EOF
 */
void skipBlank()
{
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
  {
    readChar();
  }
}

/**
 * Bỏ qua comment có dạng (* ... *)
 * Hỗ trợ comment lồng nhau: (* outer (* inner *) outer *)
 * - level tăng khi gặp (*
 * - level giảm khi gặp *)
 * - Dừng khi level = 0
 * - Báo lỗi nếu gặp EOF trước khi đóng comment
 */
void skipComment()
{
  int level = 1;
  readChar(); // Bỏ qua '*' sau '('

  while (level > 0 && currentChar != EOF)
  {
    if (currentChar == '(')
    {
      readChar();
      if (currentChar == '*')
      {
        level++; // Bắt đầu comment lồng
        readChar();
      }
    }
    else if (currentChar == '*')
    {
      readChar();
      if (currentChar == ')')
      {
        level--; // Kết thúc một level comment
        readChar();
      }
    }
    else
    {
      readChar();
    }
  }

  if (level > 0 && currentChar == EOF)
  {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }
}

/**
 * Đọc identifier hoặc keyword
 * Format: chữ cái + (chữ cái | chữ số)*
 * Giới hạn: MAX_IDENT_LEN (15 ký tự)
 * - Nếu là keyword (PROGRAM, BEGIN, ...) → trả về KW_xxx
 * - Nếu không phải keyword → trả về TK_IDENT
 */
Token *readIdentKeyword(void)
{
  Token *token;
  int ln = lineNo; // Lưu vị trí bắt đầu token
  int cn = colNo;
  char string[MAX_IDENT_LEN + 1];
  int count = 0;

  // Đọc chữ cái đầu tiên
  string[count++] = currentChar;
  readChar();

  // Đọc các chữ cái hoặc chữ số tiếp theo
  while (currentChar != EOF &&
         (charCodes[currentChar] == CHAR_LETTER ||
          charCodes[currentChar] == CHAR_DIGIT))
  {
    if (count >= MAX_IDENT_LEN)
    {
      error(ERR_IDENTTOOLONG, lineNo, colNo);
      return makeToken(TK_NONE, ln, cn);
    }
    string[count++] = currentChar;
    readChar();
  }

  string[count] = '\0';

  // Kiểm tra xem có phải keyword không
  TokenType tokenType = checkKeyword(string);

  if (tokenType != TK_NONE)
  {
    // Là keyword
    token = makeToken(tokenType, ln, cn);
  }
  else
  {
    // Là identifier
    token = makeToken(TK_IDENT, ln, cn);
  }

  strcpy(token->string, string);
  return token;
}

/**
 * Đọc số nguyên
 * Format: (chữ số)+
 * Ví dụ: 123, 0, 999
 * Giới hạn: MAX_IDENT_LEN (15 ký tự)
 */
Token *readNumber(void)
{
  Token *token;
  int ln = lineNo; // Lưu vị trí bắt đầu token
  int cn = colNo;
  char string[MAX_IDENT_LEN + 1];
  int count = 0;

  // Đọc các chữ số liên tiếp
  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
  {
    if (count >= MAX_IDENT_LEN)
    {
      error(ERR_IDENTTOOLONG, lineNo, colNo);
      return makeToken(TK_NONE, ln, cn);
    }
    string[count++] = currentChar;
    readChar();
  }

  string[count] = '\0';

  token = makeToken(TK_NUMBER, ln, cn);
  strcpy(token->string, string);
  return token;
}

/**
 * Đọc ký tự hằng số
 * Format: 'c' (đúng 1 ký tự trong dấu nháy đơn)
 * Ví dụ hợp lệ: 'a', '1', ' '
 * Ví dụ không hợp lệ: '', 'ab', "a"
 */
Token *readConstChar(void)
{
  Token *token;
  int ln = lineNo; // Lưu vị trí bắt đầu token (vị trí dấu nháy đơn mở)
  int cn = colNo;
  char string[MAX_IDENT_LEN + 1];

  readChar(); // Bỏ qua dấu nháy đơn mở

  // Kiểm tra ký tự không hợp lệ
  if (currentChar == EOF || currentChar == '\'' || currentChar == '\n')
  {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
    return makeToken(TK_NONE, ln, cn);
  }

  string[0] = currentChar;
  string[1] = '\0';
  readChar();

  // Kiểm tra phải có dấu nháy đơn đóng
  if (currentChar != '\'')
  {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
    return makeToken(TK_NONE, ln, cn);
  }

  readChar(); // Bỏ qua dấu nháy đơn đóng

  token = makeToken(TK_CHAR, ln, cn);
  strcpy(token->string, string);
  return token;
}

/**
 * Hàm chính để lấy token tiếp theo từ input stream
 * - Bỏ qua khoảng trắng và comment
 * - Nhận diện và trả về token phù hợp
 * - Báo lỗi nếu gặp ký tự không hợp lệ
 */
Token *getToken(void)
{
  Token *token;
  int ln, cn; // Biến tạm để lưu vị trí cho các token nhiều ký tự

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken(); // Bỏ qua khoảng trắng
  case CHAR_LETTER:
    return readIdentKeyword(); // Identifier hoặc keyword
  case CHAR_DIGIT:
    return readNumber(); // Số
  case CHAR_PLUS:        // Toán tử +
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=')
    {
      token = makeToken(SB_LE, ln, cn);
      readChar();
    }
    else
    {
      token = makeToken(SB_LT, ln, cn);
    }
    return token;
  case CHAR_GT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=')
    {
      token = makeToken(SB_GE, ln, cn);
      readChar();
    }
    else
    {
      token = makeToken(SB_GT, ln, cn);
    }
    return token;
  case CHAR_EXCLAIMATION:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=')
    {
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
      return token;
    }
    else
    {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token;
    }
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERIOD:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == ')')
    {
      token = makeToken(SB_RSEL, ln, cn);
      readChar();
    }
    else
    {
      token = makeToken(SB_PERIOD, ln, cn);
    }
    return token;
  case CHAR_COLON:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=')
    {
      token = makeToken(SB_ASSIGN, ln, cn);
      readChar();
    }
    else
    {
      token = makeToken(SB_COLON, ln, cn);
    }
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  case CHAR_LPAR:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '*')
    {
      skipComment();
      return getToken();
    }
    else if (currentChar == '.')
    {
      token = makeToken(SB_LSEL, ln, cn);
      readChar();
      return token;
    }
    else
    {
      token = makeToken(SB_LPAR, ln, cn);
      return token;
    }
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

/******************************************************************/

/**
 * In token ra màn hình theo định dạng: lineNo-colNo:TOKEN_TYPE
 * Ví dụ: 1-1:KW_PROGRAM, 1-9:TK_IDENT(demo), 2-5:TK_NUMBER(123)
 */
void printToken(Token *token)
{

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    printf("TK_NONE\n");
    break;
  case TK_IDENT:
    printf("TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    printf("TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    printf("TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    printf("TK_EOF\n");
    break;

  case KW_PROGRAM:
    printf("KW_PROGRAM\n");
    break;
  case KW_CONST:
    printf("KW_CONST\n");
    break;
  case KW_TYPE:
    printf("KW_TYPE\n");
    break;
  case KW_VAR:
    printf("KW_VAR\n");
    break;
  case KW_INTEGER:
    printf("KW_INTEGER\n");
    break;
  case KW_CHAR:
    printf("KW_CHAR\n");
    break;
  case KW_ARRAY:
    printf("KW_ARRAY\n");
    break;
  case KW_OF:
    printf("KW_OF\n");
    break;
  case KW_FUNCTION:
    printf("KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    printf("KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    printf("KW_BEGIN\n");
    break;
  case KW_END:
    printf("KW_END\n");
    break;
  case KW_CALL:
    printf("KW_CALL\n");
    break;
  case KW_IF:
    printf("KW_IF\n");
    break;
  case KW_THEN:
    printf("KW_THEN\n");
    break;
  case KW_ELSE:
    printf("KW_ELSE\n");
    break;
  case KW_WHILE:
    printf("KW_WHILE\n");
    break;
  case KW_DO:
    printf("KW_DO\n");
    break;
  case KW_FOR:
    printf("KW_FOR\n");
    break;
  case KW_TO:
    printf("KW_TO\n");
    break;

  case SB_SEMICOLON:
    printf("SB_SEMICOLON\n");
    break;
  case SB_COLON:
    printf("SB_COLON\n");
    break;
  case SB_PERIOD:
    printf("SB_PERIOD\n");
    break;
  case SB_COMMA:
    printf("SB_COMMA\n");
    break;
  case SB_ASSIGN:
    printf("SB_ASSIGN\n");
    break;
  case SB_EQ:
    printf("SB_EQ\n");
    break;
  case SB_NEQ:
    printf("SB_NEQ\n");
    break;
  case SB_LT:
    printf("SB_LT\n");
    break;
  case SB_LE:
    printf("SB_LE\n");
    break;
  case SB_GT:
    printf("SB_GT\n");
    break;
  case SB_GE:
    printf("SB_GE\n");
    break;
  case SB_PLUS:
    printf("SB_PLUS\n");
    break;
  case SB_MINUS:
    printf("SB_MINUS\n");
    break;
  case SB_TIMES:
    printf("SB_TIMES\n");
    break;
  case SB_SLASH:
    printf("SB_SLASH\n");
    break;
  case SB_LPAR:
    printf("SB_LPAR\n");
    break;
  case SB_RPAR:
    printf("SB_RPAR\n");
    break;
  case SB_LSEL:
    printf("SB_LSEL\n");
    break;
  case SB_RSEL:
    printf("SB_RSEL\n");
    break;
  }
}

/**
 * Hàm driver chính của scanner
 * - Mở file đầu vào
 * - Lặp lấy token cho đến EOF
 * - In từng token ra màn hình
 * - Giải phóng bộ nhớ và đóng file
 */
int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

// int main(int argc, char *argv[]) {
//   if (argc <= 1) {
//     printf("scanner: no input file.\n");
//     return -1;
//   }

//   if (scan(argv[1]) == IO_ERROR) {
//     printf("Can\'t read input file!\n");
//     return -1;
//   }

//   return 0;
// }
