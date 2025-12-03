/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"
#include "scanner.h"

extern int lineNo;      // Số dòng hiện tại (từ reader.c)
extern int colNo;       // Số cột hiện tại (từ reader.c)
extern int currentChar; // Ký tự hiện tại đang đọc (từ reader.c)

extern CharCode charCodes[]; // Mảng ánh xạ ASCII -> CharCode (từ charcode.c)

/***************************************************************/

/**
 * Bỏ qua các ký tự khoảng trắng (space, tab, newline)
 *
 * Đọc liên tục cho đến khi gặp:
 * - Ký tự không phải khoảng trắng
 * - Hoặc EOF
 *
 * Ví dụ:
 *   Input: "   PROGRAM"
 *   Sau skipBlank(): currentChar = 'P'
 *
 * Sử dụng:
 *   - Được gọi trong getToken() khi gặp CHAR_SPACE
 *   - Giúp loại bỏ whitespace không cần thiết
 */
void skipBlank()
{
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE))
    readChar();
}

/**
 * Bỏ qua comment (* ... *)
 *
 * Comment trong KPL có dạng (* text *) và có thể lồng nhau
 * Ví dụ: (* outer (* inner *) outer *)
 *
 * Thuật toán state machine:
 *   state = 0: Đang đọc bình thường
 *   state = 1: Vừa gặp '*' (có thể là kết thúc comment)
 *   state = 2: Gặp '*)' - kết thúc comment
 *
 * Luồng hoạt động:
 *   1. Đọc ký tự cho đến khi gặp '*'  → state = 1
 *   2. Nếu ký tự tiếp theo là ')'     → state = 2 (kết thúc)
 *   3. Nếu không phải ')'             → state = 0 (tiếp tục)
 *   4. Lặp lại cho đến state = 2 hoặc EOF
 *
 * Ví dụ:
 *   Input: "(* This is a comment *) PROGRAM"
 *   Sau skipComment(): currentChar = ' ' (trước PROGRAM)
 *
 * Lỗi:
 *   - Nếu EOF mà chưa gặp '*)'  → ERR_ENDOFCOMMENT
 */
void skipComment()
{
  int state = 0;
  while ((currentChar != EOF) && (state < 2))
  {
    switch (charCodes[currentChar])
    {
    case CHAR_TIMES:
      state = 1; // Gặp '*', có thể là kết thúc comment
      break;
    case CHAR_RPAR:
      if (state == 1)
        state = 2; // '*)' - kết thúc comment
      else
        state = 0;
      break;
    default:
      state = 0; // Ký tự thông thường
    }
    readChar();
  }
  if (state != 2)
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

/**
 * Đọc identifier hoặc keyword
 *
 * Format: chữ cái đầu tiên, theo sau bởi chữ cái hoặc chữ số
 * Giới hạn: tối đa 15 ký tự (MAX_IDENT_LEN)
 *
 * Thuật toán:
 *   1. Đọc ký tự đầu tiên (đã biết là chữ cái)
 *   2. Tiếp tục đọc các chữ cái/chữ số tiếp theo
 *   3. Kiểm tra độ dài (> 15 → lỗi ERR_IDENTTOOLONG)
 *   4. Gọi checkKeyword() để phân biệt keyword vs identifier
 *
 * @return Token với tokenType = KW_xxx (nếu là keyword)
 *                              hoặc TK_IDENT (nếu là identifier)
 *
 * Ví dụ:
 *   Input: "PROGRAM"  → Token(KW_PROGRAM, "PROGRAM")
 *   Input: "myVar123" → Token(TK_IDENT, "myVar123")
 *   Input: "VeryLongIdentifierName" → ERR_IDENTTOOLONG
 *
 * Lưu ý:
 *   - Sau khi đọc xong, currentChar = ký tự tiếp theo (không phải chữ cái/số)
 *   - Token cần được free() sau khi dùng
 */
Token *readIdentKeyword(void)
{
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int count = 1;

  token->string[0] = (char)currentChar;
  readChar();

  while ((currentChar != EOF) &&
         ((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT)))
  {
    if (count <= MAX_IDENT_LEN)
      token->string[count++] = (char)currentChar;
    readChar();
  }

  if (count > MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
    return token;
  }

  token->string[count] = '\0';
  token->tokenType = checkKeyword(token->string);

  if (token->tokenType == TK_NONE)
    token->tokenType = TK_IDENT;

  return token;
}

/**
 * Đọc số nguyên (chuỗi các chữ số)
 *
 * Format: một hoặc nhiều chữ số (0-9)
 *
 * Thuật toán:
 *   1. Đọc liên tiếp các chữ số
 *   2. Lưu vào token->string
 *   3. Chuyển đổi thành số nguyên (atoi) và lưu vào token->value
 *
 * @return Token với tokenType = TK_NUMBER
 *
 * Ví dụ:
 *   Input: "123"   → Token(TK_NUMBER, "123", value=123)
 *   Input: "0"     → Token(TK_NUMBER, "0", value=0)
 *   Input: "99999" → Token(TK_NUMBER, "99999", value=99999)
 *
 * Lưu ý:
 *   - Sau khi đọc xong, currentChar = ký tự tiếp theo (không phải chữ số)
 *   - Không hỗ trợ số âm (dấu '-' được xử lý riêng trong parser)
 *   - Không hỗ trợ số thực (chỉ có số nguyên)
 */
Token *readNumber(void)
{
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int count = 0;

  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT))
  {
    token->string[count++] = (char)currentChar;
    readChar();
  }

  token->string[count] = '\0';
  token->value = atoi(token->string);
  return token;
}

/**
 * Đọc hằng ký tự (char constant)
 *
 * Format: 'c' - một ký tự duy nhất trong dấu nháy đơn
 *
 * Thuật toán:
 *   1. Đọc dấu nháy mở '   (đã được xử lý trước đó)
 *   2. Đọc ký tự bên trong
 *   3. Kiểm tra dấu nháy đóng '
 *
 * @return Token với tokenType = TK_CHAR (nếu hợp lệ)
 *                              hoặc TK_NONE (nếu lỗi)
 *
 * Ví dụ hợp lệ:
 *   Input: "'a'"  → Token(TK_CHAR, "a")
 *   Input: "'Z'"  → Token(TK_CHAR, "Z")
 *   Input: "'5'"  → Token(TK_CHAR, "5")
 *
 * Ví dụ lỗi:
 *   Input: "'ab'" → ERR_INVALIDCHARCONSTANT (nhiều hơn 1 ký tự)
 *   Input: "'a"   → ERR_INVALIDCHARCONSTANT (thiếu dấu nháy đóng)
 *   Input: "'"    → ERR_INVALIDCHARCONSTANT (không có ký tự)
 *
 * Lưu ý:
 *   - KPL không hỗ trợ escape sequences (\n, \t, ...)
 *   - Chỉ chấp nhận đúng 1 ký tự giữa hai dấu nháy đơn
 */
Token *readConstChar(void)
{
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar(); // Đọc ký tự sau dấu nháy mở
  if (currentChar == EOF)
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  token->string[0] = currentChar;
  token->string[1] = '\0';

  readChar(); // Đọc ký tự tiếp theo (phải là dấu nháy đóng)
  if (currentChar == EOF)
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
  {
    readChar(); // Tiêu thụ dấu nháy đóng
    return token;
  }
  else
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}

/**
 * Hàm chính của Scanner - lấy token tiếp theo từ input
 *
 * Đây là state machine xử lý từng loại ký tự:
 * - CHAR_SPACE     → skipBlank() và gọi đệ quy getToken()
 * - CHAR_LETTER    → readIdentKeyword()
 * - CHAR_DIGIT     → readNumber()
 * - CHAR_PLUS      → SB_PLUS
 * - CHAR_MINUS     → SB_MINUS
 * - CHAR_TIMES     → SB_TIMES
 * - CHAR_SLASH     → SB_SLASH
 * - CHAR_LT        → SB_LT hoặc SB_LE (nếu theo sau là '=')
 * - CHAR_GT        → SB_GT hoặc SB_GE (nếu theo sau là '=')
 * - CHAR_EQ        → SB_EQ
 * - CHAR_EXCLAIMATION → SB_NEQ (nếu theo sau là '='), ngược lại lỗi
 * - CHAR_COMMA     → SB_COMMA
 * - CHAR_PERIOD    → SB_PERIOD hoặc SB_RSEL (nếu theo sau là ')')
 * - CHAR_SEMICOLON → SB_SEMICOLON
 * - CHAR_COLON     → SB_COLON hoặc SB_ASSIGN (nếu theo sau là '=')
 * - CHAR_SINGLEQUOTE → readConstChar()
 * - CHAR_LPAR      → SB_LPAR, SB_LSEL (nếu '(.'), hoặc skipComment() (nếu '(*')
 * - CHAR_RPAR      → SB_RPAR
 * - EOF            → TK_EOF
 * - Unknown        → ERR_INVALIDSYMBOL
 *
 * @return Token mới được tạo (cần free() sau khi dùng)
 *
 * Ví dụ luồng xử lý:
 *   Input: "PROGRAM demo;"
 *
 *   getToken() #1:
 *     currentChar = 'P' → CHAR_LETTER → readIdentKeyword()
 *     → Token(KW_PROGRAM, "PROGRAM", 1, 1)
 *
 *   getToken() #2:
 *     currentChar = ' ' → CHAR_SPACE → skipBlank() → getToken() (đệ quy)
 *     currentChar = 'd' → CHAR_LETTER → readIdentKeyword()
 *     → Token(TK_IDENT, "demo", 1, 9)
 *
 *   getToken() #3:
 *     currentChar = ';' → CHAR_SEMICOLON
 *     → Token(SB_SEMICOLON, ";", 1, 13)
 *
 * Lưu ý:
 *   - Hàm này XỬ LÝ whitespace/comment (gọi skipBlank/skipComment và đệ quy)
 *   - Parser thường dùng getValidToken() thay vì hàm này
 *   - Một số token cần look-ahead 1 ký tự (<=, >=, !=, :=, (., .)
 */
Token *getToken(void)
{
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();

  // Toán tử số học đơn giản
  case CHAR_PLUS:
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

  // Toán tử so sánh (cần look-ahead)
  case CHAR_LT: // '<' hoặc '<='
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_LE, ln, cn); // '<='
    }
    else
      return makeToken(SB_LT, ln, cn); // '<'

  case CHAR_GT: // '>' hoặc '>='
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_GE, ln, cn); // '>='
    }
    else
      return makeToken(SB_GT, ln, cn); // '>'

  case CHAR_EQ: // '='
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;

  case CHAR_EXCLAIMATION: // '!' phải theo sau bởi '=' → '!='
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_NEQ, ln, cn); // '!='
    }
    else
    {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn); // '!' đơn lẻ là lỗi
      return token;
    }

  // Dấu phân cách
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;

  case CHAR_PERIOD: // '.' hoặc '.)'
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR))
    {
      readChar();
      return makeToken(SB_RSEL, ln, cn); // '.)' - array selector
    }
    else
      return makeToken(SB_PERIOD, ln, cn); // '.'

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;

  case CHAR_COLON: // ':' hoặc ':='
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn); // ':=' - assignment
    }
    else
      return makeToken(SB_COLON, ln, cn); // ':'

  // Ký tự đặc biệt
  case CHAR_SINGLEQUOTE:
    return readConstChar(); // 'c'

  case CHAR_LPAR: // '(', '(*', hoặc '(.'
    ln = lineNo;
    cn = colNo;
    readChar();

    if (currentChar == EOF)
      return makeToken(SB_LPAR, ln, cn);

    switch (charCodes[currentChar])
    {
    case CHAR_PERIOD: // '(.' - array selector
      readChar();
      return makeToken(SB_LSEL, ln, cn);
    case CHAR_TIMES: // '(*' - comment
      readChar();
      skipComment();
      return getToken(); // Đệ quy để lấy token tiếp theo
    default:
      return makeToken(SB_LPAR, ln, cn); // '('
    }

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;

  default: // Ký tự không hợp lệ
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

/**
 * Lấy token hợp lệ tiếp theo (bỏ qua các token lỗi TK_NONE)
 *
 * Hàm này gọi getToken() liên tục cho đến khi:
 * - Gặp token hợp lệ (tokenType != TK_NONE)
 * - Hoặc gặp EOF
 *
 * @return Token hợp lệ (cần free() sau khi dùng)
 *
 * Ví dụ:
 *   Input có lỗi: "PROGRAM ! demo;"
 *
 *   getValidToken() #1:
 *     getToken() → Token(KW_PROGRAM) ✅ → trả về
 *
 *   getValidToken() #2:
 *     getToken() → Token(TK_NONE) ❌ (lỗi '!') → free và gọi lại
 *     getToken() → Token(TK_IDENT("demo")) ✅ → trả về
 *
 * Sử dụng:
 *   - Parser dùng hàm này để lấy lookAhead token
 *   - Đảm bảo chỉ nhận token có ý nghĩa (không phải lỗi)
 *
 * Lưu ý:
 *   - Các token lỗi (TK_NONE) đã được báo lỗi trong getToken()
 *   - Hàm này chỉ bỏ qua và tiếp tục lấy token tiếp theo
 */
Token *getValidToken(void)
{
  Token *token = getToken();
  while (token->tokenType == TK_NONE)
  {
    free(token);
    token = getToken();
  }
  return token;
}

/******************************************************************/

/**
 * In thông tin token ra màn hình (dùng để debug/test)
 *
 * Format output: <lineNo>-<colNo>:<tokenType>[(string)]
 *
 * @param token Con trỏ tới token cần in
 *
 * Ví dụ output:
 *   Token cơ bản:
 *     Token(TK_IDENT, "demo", 1, 9)     → "1-9:TK_IDENT(demo)"
 *     Token(TK_NUMBER, "123", 2, 5)     → "2-5:TK_NUMBER(123)"
 *     Token(TK_CHAR, "a", 3, 10)        → "3-10:TK_CHAR('a')"
 *     Token(TK_EOF, "", 10, 1)          → "10-1:TK_EOF"
 *
 *   Keywords:
 *     Token(KW_PROGRAM, "", 1, 1)       → "1-1:KW_PROGRAM"
 *     Token(KW_BEGIN, "", 5, 1)         → "5-1:KW_BEGIN"
 *     Token(KW_IF, "", 7, 3)            → "7-3:KW_IF"
 *
 *   Symbols:
 *     Token(SB_SEMICOLON, "", 1, 13)    → "1-13:SB_SEMICOLON"
 *     Token(SB_ASSIGN, "", 8, 5)        → "8-5:SB_ASSIGN"
 *     Token(SB_PLUS, "", 9, 10)         → "9-10:SB_PLUS"
 *
 * Sử dụng:
 *   - Scanner test: in tất cả token từ file
 *   - Parser: in token đang được xử lý (trong hàm eat())
 *   - So sánh output với expected result
 *
 * Lưu ý:
 *   - Chỉ TK_IDENT, TK_NUMBER, TK_CHAR có in thêm string/value
 *   - Các token khác chỉ in tokenType
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
