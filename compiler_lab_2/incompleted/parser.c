/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @version 1.0
 *
 * Module parser.c thực hiện phân tích cú pháp (syntax analysis)
 * của chương trình nguồn theo kỹ thuật Recursive Descent Parsing.
 *
 * currentToken: token hiện tại đang được xử lý
 * lookAhead: token tiếp theo
 */

#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "parser.h"
#include "reader.h"
#include "scanner.h"

Token *currentToken; // Token hiện tại đang được phân tích
Token *lookAhead;    // Token tiếp theo (peek token) - dùng để xác định luồng phân tích

/* ================== Hàm hỗ trợ ================== */

/**
 * Hàm scan: Di chuyển sang token tiếp theo
 * - Giải phóng currentToken cũ
 * - Chuyển lookAhead thành currentToken
 * - Lấy token mới cho lookAhead
 */
void scan(void)
{
  Token *tmp = currentToken;   // Lưu lại token hiện tại để giải phóng
  currentToken = lookAhead;    // Chuyển lookAhead thành currentToken
  lookAhead = getValidToken(); // Lấy token hợp lệ mới từ scanner
  free(tmp);                   // Giải phóng bộ nhớ token cũ
}

/**
 * Hàm eat: Kiểm tra và "ăn" token mong đợi
 * @param tokenType: Loại token mong đợi
 * - Nếu lookAhead khớp với tokenType: in token và chuyển sang token tiếp theo
 * - Nếu không khớp: báo lỗi thiếu token
 */
void eat(TokenType tokenType)
{
  if (lookAhead->tokenType == tokenType)
  {
    printToken(lookAhead); // In token ra console để debug
    scan();                // Chuyển sang token tiếp theo
  }
  else
    // Báo lỗi thiếu token mong đợi
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

/* ================== Phân tích chương trình ================== */

/**
 * Hàm compileProgram: Phân tích toàn bộ chương trình
 * Cấu trúc: PROGRAM <tên> ; <block> .
 */
void compileProgram(void)
{
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);   // Keyword 'PROGRAM'
  eat(TK_IDENT);     // Tên chương trình (identifier)
  eat(SB_SEMICOLON); // Dấu ';'
  compileBlock();    // Phân tích block chính của chương trình
  eat(SB_PERIOD);    // Dấu '.' kết thúc chương trình
  assert("Program parsed!");
}

/**
 * Hàm compileBlock: Phân tích block chương trình
 * Block bao gồm: const declarations, type declarations, var declarations,
 *                subroutine declarations và statements
 */
void compileBlock(void)
{
  assert("Parsing a Block ....");
  // Kiểm tra nếu có khai báo CONST
  if (lookAhead->tokenType == KW_CONST)
  {
    eat(KW_CONST);       // Ăn keyword CONST
    compileConstDecl();  // Phân tích khai báo const đầu tiên
    compileConstDecls(); // Phân tích các khai báo const còn lại
    compileBlock2();     // Tiếp tục phân tích các phần còn lại của block
  }
  else
    compileBlock2(); // Không có CONST, chuyển sang phần tiếp theo
  assert("Block parsed!");
}

/**
 * Hàm compileBlock2: Phân tích type declarations
 */
void compileBlock2(void)
{
  // Kiểm tra nếu có khai báo TYPE
  if (lookAhead->tokenType == KW_TYPE)
  {
    eat(KW_TYPE);       // Ăn keyword TYPE
    compileTypeDecl();  // Phân tích khai báo type đầu tiên
    compileTypeDecls(); // Phân tích các khai báo type còn lại
    compileBlock3();    // Tiếp tục sang var declarations
  }
  else
    compileBlock3(); // Không có TYPE, chuyển sang var
}

/**
 * Hàm compileBlock3: Phân tích var declarations
 */
void compileBlock3(void)
{
  // Kiểm tra nếu có khai báo VAR
  if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);       // Ăn keyword VAR
    compileVarDecl();  // Phân tích khai báo var đầu tiên
    compileVarDecls(); // Phân tích các khai báo var còn lại
    compileBlock4();   // Tiếp tục sang subroutine declarations
  }
  else
    compileBlock4(); // Không có VAR, chuyển sang subroutine
}

/**
 * Hàm compileBlock4: Phân tích subroutine declarations (functions/procedures)
 */
void compileBlock4(void)
{
  compileSubDecls(); // Phân tích các khai báo function/procedure
  compileBlock5();   // Tiếp tục sang phần statements
}

/**
 * Hàm compileBlock5: Phân tích phần thân chương trình (statements)
 * Cấu trúc: BEGIN <statements> END
 */
void compileBlock5(void)
{
  eat(KW_BEGIN);       // Keyword BEGIN
  compileStatements(); // Phân tích các câu lệnh
  eat(KW_END);         // Keyword END
}

/* ================== Phân tích khai báo ================== */

/**
 * Hàm compileConstDecls: Phân tích nhiều khai báo const
 * Lặp lại cho đến khi không còn identifier
 */
void compileConstDecls(void)
{
  // Tiếp tục phân tích nếu còn identifier (tên const mới)
  while (lookAhead->tokenType == TK_IDENT)
    compileConstDecl();
}

/**
 * Hàm compileConstDecl: Phân tích một khai báo const
 * Cấu trúc: <tên> = <giá trị> ;
 */
void compileConstDecl(void)
{
  eat(TK_IDENT);     // Tên hằng số
  eat(SB_EQ);        // Dấu '='
  compileConstant(); // Giá trị của hằng số
  eat(SB_SEMICOLON); // Dấu ';'
}

/**
 * Hàm compileTypeDecls: Phân tích nhiều khai báo type
 */
void compileTypeDecls(void)
{
  // Tiếp tục phân tích nếu còn identifier (tên type mới)
  while (lookAhead->tokenType == TK_IDENT)
    compileTypeDecl();
}

/**
 * Hàm compileTypeDecl: Phân tích một khai báo type
 * Cấu trúc: <tên> = <kiểu dữ liệu> ;
 */
void compileTypeDecl(void)
{
  eat(TK_IDENT);     // Tên kiểu dữ liệu mới
  eat(SB_EQ);        // Dấu '='
  compileType();     // Định nghĩa kiểu dữ liệu
  eat(SB_SEMICOLON); // Dấu ';'
}

/**
 * Hàm compileVarDecls: Phân tích nhiều khai báo biến
 */
void compileVarDecls(void)
{
  // Tiếp tục phân tích nếu còn identifier (tên biến mới)
  while (lookAhead->tokenType == TK_IDENT)
    compileVarDecl();
}

/**
 * Hàm compileVarDecl: Phân tích một khai báo biến
 * Cấu trúc: <tên biến> : <kiểu> ;
 */
void compileVarDecl(void)
{
  eat(TK_IDENT);     // Tên biến
  eat(SB_COLON);     // Dấu ':'
  compileType();     // Kiểu dữ liệu của biến
  eat(SB_SEMICOLON); // Dấu ';'
}

/**
 * Hàm compileSubDecls: Phân tích các khai báo subroutine (function/procedure)
 * Sử dụng đệ quy để phân tích nhiều subroutine liên tiếp
 */
void compileSubDecls(void)
{
  assert("Parsing subtoutines ....");
  switch (lookAhead->tokenType)
  {
  case KW_FUNCTION:    // Nếu là function
    compileFuncDecl(); // Phân tích function
    compileSubDecls(); // Tiếp tục phân tích subroutine khác (đệ quy)
    break;
  case KW_PROCEDURE:   // Nếu là procedure
    compileProcDecl(); // Phân tích procedure
    compileSubDecls(); // Tiếp tục phân tích subroutine khác (đệ quy)
    break;
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break; // FOLLOW set - kết thúc danh sách subroutine
  }
  assert("Subtoutines parsed ....");
}

/**
 * Hàm compileFuncDecl: Phân tích khai báo function
 * Cấu trúc: FUNCTION <tên>(<params>) : <kiểu trả về> ; <block> ;
 */
void compileFuncDecl(void)
{
  assert("Parsing a function ....");
  eat(KW_FUNCTION);   // Keyword FUNCTION
  eat(TK_IDENT);      // Tên function
  compileParams();    // Danh sách tham số
  eat(SB_COLON);      // Dấu ':'
  compileBasicType(); // Kiểu trả về (INTEGER hoặc CHAR)
  eat(SB_SEMICOLON);  // Dấu ';'
  compileBlock();     // Thân function
  eat(SB_SEMICOLON);  // Dấu ';' kết thúc
  assert("Function parsed ....");
}

/**
 * Hàm compileProcDecl: Phân tích khai báo procedure
 * Cấu trúc: PROCEDURE <tên>(<params>) ; <block> ;
 */
void compileProcDecl(void)
{
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE); // Keyword PROCEDURE
  eat(TK_IDENT);     // Tên procedure
  compileParams();   // Danh sách tham số
  eat(SB_SEMICOLON); // Dấu ';'
  compileBlock();    // Thân procedure
  eat(SB_SEMICOLON); // Dấu ';' kết thúc
  assert("Procedure parsed ....");
}

/* ================== Phân tích hằng số và kiểu ================== */

/**
 * Hàm compileUnsignedConstant: Phân tích hằng số không dấu
 * Có thể là: số (TK_NUMBER), ký tự (TK_CHAR), hoặc tên hằng (TK_IDENT)
 */
void compileUnsignedConstant(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER: // Hằng số nguyên
  case TK_IDENT:  // Tên hằng đã khai báo trước
  case TK_CHAR:   // Hằng ký tự
    eat(lookAhead->tokenType);
    break;
  default:
    // Báo lỗi nếu không phải hằng số hợp lệ
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileConstant: Phân tích hằng số (có thể có dấu +/-)
 * Cấu trúc: [+|-] <hằng số>
 */
void compileConstant(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_PLUS: // Dấu '+'
    eat(SB_PLUS);
    compileConstant2(); // Phân tích phần hằng số
    break;
  case SB_MINUS: // Dấu '-'
    eat(SB_MINUS);
    compileConstant2(); // Phân tích phần hằng số
    break;
  case TK_CHAR: // Ký tự không có dấu
    eat(TK_CHAR);
    break;
  default: // Hằng số không có dấu
    compileConstant2();
    break;
  }
}

/**
 * Hàm compileConstant2: Phân tích phần hằng số chính (NUMBER hoặc IDENT)
 */
void compileConstant2(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER: // Số nguyên
  case TK_IDENT:  // Tên hằng
    eat(lookAhead->tokenType);
    break;
  default:
    // Báo lỗi hằng số không hợp lệ
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileType: Phân tích định nghĩa kiểu dữ liệu
 * Có thể là: INTEGER, CHAR, ARRAY[size] OF type, hoặc tên type đã định nghĩa
 */
void compileType(void)
{
  switch (lookAhead->tokenType)
  {
  case KW_INTEGER: // Kiểu INTEGER
    eat(KW_INTEGER);
    break;
  case KW_CHAR: // Kiểu CHAR
    eat(KW_CHAR);
    break;
  case KW_ARRAY: // Kiểu mảng: ARRAY[n] OF type
    eat(KW_ARRAY);
    eat(SB_LSEL);   // Dấu '['
    eat(TK_NUMBER); // Kích thước mảng
    eat(SB_RSEL);   // Dấu ']'
    eat(KW_OF);     // Keyword OF
    compileType();  // Kiểu phần tử mảng (đệ quy cho mảng nhiều chiều)
    break;
  case TK_IDENT: // Kiểu do người dùng định nghĩa
    eat(TK_IDENT);
    break;
  default:
    // Báo lỗi kiểu không hợp lệ
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileBasicType: Phân tích kiểu cơ bản (INTEGER hoặc CHAR)
 * Dùng cho kiểu trả về của function và kiểu tham số
 */
void compileBasicType(void)
{
  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
  case KW_CHAR:
    eat(lookAhead->tokenType);
    break;
  default:
    // Báo lỗi kiểu cơ bản không hợp lệ
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/* ================== Phân tích tham số ================== */

/**
 * Hàm compileParams: Phân tích danh sách tham số
 * Cấu trúc: (<param1> ; <param2> ; ...) hoặc rỗng
 */
void compileParams(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_LPAR: // Dấu '(' - có tham số
    eat(SB_LPAR);
    compileParam();   // Phân tích tham số đầu tiên
    compileParams2(); // Phân tích các tham số còn lại
    eat(SB_RPAR);     // Dấu ')'
    break;
  case SB_SEMICOLON:
  case SB_COLON:
    break; // Không có tham số
  default:
    // Báo lỗi danh sách tham số không hợp lệ
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileParams2: Phân tích các tham số tiếp theo (sau tham số đầu tiên)
 * Cấu trúc: (; <param>)*
 */
void compileParams2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_SEMICOLON: // Dấu ';' - còn tham số tiếp theo
    eat(SB_SEMICOLON);
    compileParam();   // Phân tích tham số tiếp
    compileParams2(); // Tiếp tục phân tích (đệ quy)
    break;
  case SB_RPAR:
    break; // Dấu ')' - hết danh sách tham số
  default:
    // Báo lỗi danh sách tham số không hợp lệ
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileParam: Phân tích một tham số
 * Cấu trúc: <tên> : <kiểu> hoặc VAR <tên> : <kiểu> (tham số tham chiếu)
 */
void compileParam(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:        // Tham số truyền giá trị
    eat(TK_IDENT);      // Tên tham số
    eat(SB_COLON);      // Dấu ':'
    compileBasicType(); // Kiểu tham số
    break;
  case KW_VAR: // Tham số truyền tham chiếu
    eat(KW_VAR);
    eat(TK_IDENT);      // Tên tham số
    eat(SB_COLON);      // Dấu ':'
    compileBasicType(); // Kiểu tham số
    break;
  default:
    // Báo lỗi tham số không hợp lệ
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/* ================== Phân tích câu lệnh ================== */

/**
 * Hàm compileStatements: Phân tích danh sách câu lệnh
 * Cấu trúc: <statement> (; <statement>)*
 */
void compileStatements(void)
{
  compileStatement();   // Phân tích câu lệnh đầu tiên
  compileStatements2(); // Phân tích các câu lệnh tiếp theo
}

/**
 * Hàm compileStatements2: Phân tích các câu lệnh tiếp theo
 * Mỗi câu lệnh cách nhau bởi dấu ';'
 */
void compileStatements2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_SEMICOLON: // Dấu ';' - còn câu lệnh tiếp theo
    eat(SB_SEMICOLON);
    compileStatement();   // Phân tích câu lệnh tiếp
    compileStatements2(); // Tiếp tục (đệ quy)
    break;
  case KW_END:
  case KW_ELSE:
    break; // FOLLOW set - kết thúc danh sách câu lệnh
  default:
    // Báo lỗi thiếu dấu ';'
    missingToken(SB_SEMICOLON, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileStatement: Phân tích một câu lệnh
 * Có thể là: gán, gọi procedure, BEGIN-END, IF, WHILE, FOR, hoặc câu lệnh rỗng
 */
void compileStatement(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT: // Câu lệnh gán: biến := giá trị
    compileAssignSt();
    break;
  case KW_CALL: // Câu lệnh gọi procedure
    compileCallSt();
    break;
  case KW_BEGIN: // Nhóm câu lệnh: BEGIN ... END
    compileGroupSt();
    break;
  case KW_IF: // Câu lệnh điều kiện: IF ... THEN ... ELSE ...
    compileIfSt();
    break;
  case KW_WHILE: // Vòng lặp: WHILE ... DO ...
    compileWhileSt();
    break;
  case KW_FOR: // Vòng lặp: FOR ... TO ... DO ...
    compileForSt();
    break;
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break; // Câu lệnh rỗng
  default:
    // Báo lỗi câu lệnh không hợp lệ
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileAssignSt: Phân tích câu lệnh gán
 * Cấu trúc: <biến>[index] := <biểu thức>
 */
void compileAssignSt(void)
{
  assert("Parsing an assign statement ....");
  eat(TK_IDENT); // Tên biến
  if (lookAhead->tokenType == SB_LSEL)
    compileIndexes();  // Nếu là mảng, phân tích các chỉ số [i][j]...
  eat(SB_ASSIGN);      // Toán tử gán ':='
  compileExpression(); // Biểu thức vế phải
  assert("Assign statement parsed ....");
}

/**
 * Hàm compileCallSt: Phân tích câu lệnh gọi procedure
 * Cấu trúc: CALL <tên procedure>(<arguments>)
 */
void compileCallSt(void)
{
  assert("Parsing a call statement ....");
  eat(KW_CALL);       // Keyword CALL
  eat(TK_IDENT);      // Tên procedure
  compileArguments(); // Danh sách đối số
  assert("Call statement parsed ....");
}

/**
 * Hàm compileGroupSt: Phân tích nhóm câu lệnh
 * Cấu trúc: BEGIN <statements> END
 */
void compileGroupSt(void)
{
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);       // Keyword BEGIN
  compileStatements(); // Các câu lệnh bên trong
  eat(KW_END);         // Keyword END
  assert("Group statement parsed ....");
}

/**
 * Hàm compileIfSt: Phân tích câu lệnh IF
 * Cấu trúc: IF <condition> THEN <statement> [ELSE <statement>]
 */
void compileIfSt(void)
{
  assert("Parsing an if statement ....");
  eat(KW_IF);         // Keyword IF
  compileCondition(); // Điều kiện
  eat(KW_THEN);       // Keyword THEN
  compileStatement(); // Câu lệnh trong nhánh THEN
  if (lookAhead->tokenType == KW_ELSE)
    compileElseSt(); // Nhánh ELSE (nếu có)
  assert("If statement parsed ....");
}

/**
 * Hàm compileElseSt: Phân tích nhánh ELSE
 */
void compileElseSt(void)
{
  eat(KW_ELSE);       // Keyword ELSE
  compileStatement(); // Câu lệnh trong nhánh ELSE
}

/**
 * Hàm compileWhileSt: Phân tích vòng lặp WHILE
 * Cấu trúc: WHILE <condition> DO <statement>
 */
void compileWhileSt(void)
{
  assert("Parsing a while statement ....");
  eat(KW_WHILE);      // Keyword WHILE
  compileCondition(); // Điều kiện lặp
  eat(KW_DO);         // Keyword DO
  compileStatement(); // Thân vòng lặp
  assert("While statement parsed ....");
}

/**
 * Hàm compileForSt: Phân tích vòng lặp FOR
 * Cấu trúc: FOR <biến> := <expr> TO <expr> DO <statement>
 */
void compileForSt(void)
{
  assert("Parsing a for statement ....");
  eat(KW_FOR);         // Keyword FOR
  eat(TK_IDENT);       // Biến đếm
  eat(SB_ASSIGN);      // Dấu ':='
  compileExpression(); // Giá trị bắt đầu
  eat(KW_TO);          // Keyword TO
  compileExpression(); // Giá trị kết thúc
  eat(KW_DO);          // Keyword DO
  compileStatement();  // Thân vòng lặp
  assert("For statement parsed ....");
}

/* ================== Phân tích biểu thức và toán hạng ================== */

/**
 * Hàm compileArguments: Phân tích danh sách đối số khi gọi hàm/procedure
 * Cấu trúc: (<expr1>, <expr2>, ...) hoặc rỗng
 */
void compileArguments(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_LPAR: // Dấu '(' - có đối số
    eat(SB_LPAR);
    compileExpression(); // Đối số đầu tiên
    compileArguments2(); // Các đối số tiếp theo
    eat(SB_RPAR);        // Dấu ')'
    break;
  // FOLLOW set - không có đối số
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case SB_TIMES:
  case SB_SLASH:
  case KW_TO:
  case KW_DO:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RPAR:
  case SB_RSEL:
  case KW_THEN:
    break;
  default:
    // Báo lỗi danh sách đối số không hợp lệ
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileArguments2: Phân tích các đối số tiếp theo
 * Cấu trúc: (, <expr>)*
 */
void compileArguments2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_COMMA: // Dấu ',' - còn đối số tiếp theo
    eat(SB_COMMA);
    compileExpression(); // Đối số tiếp
    compileArguments2(); // Tiếp tục (đệ quy)
    break;
  case SB_RPAR:
    break; // Dấu ')' - hết danh sách đối số
  default:
    // Báo lỗi danh sách đối số không hợp lệ
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileCondition: Phân tích điều kiện (biểu thức so sánh)
 * Cấu trúc: <expr> <toán tử so sánh> <expr>
 */
void compileCondition(void)
{
  compileExpression(); // Biểu thức vế trái
  compileCondition2(); // Toán tử so sánh và biểu thức vế phải
}

/**
 * Hàm compileCondition2: Phân tích toán tử so sánh và biểu thức vế phải
 * Toán tử: =, <>, <=, <, >=, >
 */
void compileCondition2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_EQ:  // Bằng '='
  case SB_NEQ: // Khác '<>'
  case SB_LE:  // Nhỏ hơn hoặc bằng '<='
  case SB_LT:  // Nhỏ hơn '<'
  case SB_GE:  // Lớn hơn hoặc bằng '>='
  case SB_GT:  // Lớn hơn '>'
    eat(lookAhead->tokenType);
    compileExpression(); // Biểu thức vế phải
    break;
  default:
    // Báo lỗi toán tử so sánh không hợp lệ
    error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileExpression: Phân tích biểu thức số học
 * Cấu trúc: [+|-] <term> ((+|-) <term>)*
 */
void compileExpression(void)
{
  assert("Parsing an expression");
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:  // Dấu '+' đầu biểu thức
  case SB_MINUS: // Dấu '-' đầu biểu thức (số âm)
    eat(lookAhead->tokenType);
    compileExpression2();
    break;
  default: // Biểu thức không có dấu
    compileExpression2();
    break;
  }
  assert("Expression parsed");
}

/**
 * Hàm compileExpression2: Phân tích phần chính của biểu thức
 */
void compileExpression2(void)
{
  compileTerm();        // Term đầu tiên
  compileExpression3(); // Các term tiếp theo (nếu có)
}

/**
 * Hàm compileExpression3: Phân tích các phép cộng/trừ
 * Cấu trúc: ((+|-) <term>)*
 */
void compileExpression3(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:  // Phép cộng
  case SB_MINUS: // Phép trừ
    eat(lookAhead->tokenType);
    compileTerm();        // Term tiếp theo
    compileExpression3(); // Tiếp tục (đệ quy) - để xử lý a+b+c+...
    break;
  // FOLLOW set - kết thúc biểu thức
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    // Báo lỗi biểu thức không hợp lệ
    error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

/**
 * Hàm compileTerm: Phân tích term (tích của các factor)
 * Cấu trúc: <factor> ((*|/) <factor>)*
 */
void compileTerm(void)
{
  compileFactor(); // Factor đầu tiên
  compileTerm2();  // Các factor tiếp theo (nếu có)
}

/**
 * Hàm compileTerm2: Phân tích các phép nhân/chia
 * Cấu trúc: ((*|/) <factor>)*
 */
void compileTerm2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_TIMES: // Phép nhân '*'
  case SB_SLASH: // Phép chia '/'
    eat(lookAhead->tokenType);
    compileFactor(); // Factor tiếp theo
    compileTerm2();  // Tiếp tục (đệ quy) - để xử lý a*b*c*...
    break;
  // FOLLOW set - kết thúc term
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    // Báo lỗi term không hợp lệ
    error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

/**
 * Hàm compileFactor: Phân tích factor (đơn vị cơ bản trong biểu thức)
 * Có thể là: số, ký tự, biến, mảng, gọi hàm, hoặc biểu thức trong ngoặc
 */
void compileFactor(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER: // Hằng số
  case TK_CHAR:   // Ký tự
    compileUnsignedConstant();
    break;
  case SB_LPAR: // Biểu thức trong ngoặc: (<expr>)
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
    break;
  case TK_IDENT: // Biến, mảng, hoặc gọi hàm
    eat(TK_IDENT);
    switch (lookAhead->tokenType)
    {
    case SB_LSEL: // Truy cập mảng: a[i][j]
      compileIndexes();
      break;
    case SB_LPAR: // Gọi hàm: func(args)
      compileArguments();
      break;
    default:
      break; // Chỉ là biến thông thường
    }
    break;
  default:
    // Báo lỗi factor không hợp lệ
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/**
 * Hàm compileIndexes: Phân tích các chỉ số mảng
 * Cấu trúc: ([<expr>])+ - hỗ trợ mảng nhiều chiều
 */
void compileIndexes(void)
{
  // Lặp qua tất cả các chỉ số [i][j][k]...
  while (lookAhead->tokenType == SB_LSEL)
  {
    eat(SB_LSEL);        // Dấu '['
    compileExpression(); // Biểu thức chỉ số
    eat(SB_RSEL);        // Dấu ']'
  }
}

/* ================== Hàm compile chính ================== */

/**
 * Hàm compile: Hàm chính để bắt đầu quá trình biên dịch
 * @param fileName: Đường dẫn file nguồn cần biên dịch
 * @return: IO_SUCCESS nếu thành công, IO_ERROR nếu lỗi
 */
int compile(char *fileName)
{
  // Mở file nguồn để đọc
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken(); // Lấy token đầu tiên
  compileProgram();            // Bắt đầu phân tích cú pháp từ đầu chương trình

  // Giải phóng bộ nhớ và đóng file
  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
}
