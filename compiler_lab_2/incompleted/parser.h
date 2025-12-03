/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include "token.h"

/*
 * parser.h định nghĩa các hàm của module Parser,
 * chịu trách nhiệm phân tích cú pháp (syntax analysis)
 * của chương trình nguồn.
 *
 * Các hàm này tuân theo kỹ thuật Recursive Descent Parsing,
 * mỗi hàm tương ứng với một non-terminal trong ngữ pháp KPL.
 */

/* ================== Các hàm hỗ trợ ================== */

/**
 * Lấy token tiếp theo từ scanner
 *
 * Chức năng:
 *   - Giải phóng currentToken cũ
 *   - Chuyển lookAhead → currentToken
 *   - Lấy token mới từ scanner → lookAhead
 *
 * Ví dụ:
 *   Trước scan(): currentToken = KW_PROGRAM, lookAhead = TK_IDENT("demo")
 *   Sau scan():   currentToken = TK_IDENT("demo"), lookAhead = SB_SEMICOLON
 *
 * Sử dụng:
 *   - Được gọi trong hàm eat() sau khi match token
 *   - Duy trì sliding window 2 token (currentToken, lookAhead)
 */
void scan(void);

/**
 * Kiểm tra và tiêu thụ token
 *
 * @param tokenType Loại token mong đợi (KW_xxx, TK_xxx, SB_xxx)
 *
 * Chức năng:
 *   - Nếu lookAhead khớp với tokenType:
 *     + In token ra màn hình (printToken)
 *     + Lấy token tiếp theo (scan)
 *   - Nếu không khớp:
 *     + Gọi missingToken() → báo lỗi và exit
 *
 * Ví dụ:
 *   lookAhead = SB_SEMICOLON
 *   eat(SB_SEMICOLON);  → In "1-13:SB_SEMICOLON", lấy token tiếp theo
 *
 *   lookAhead = TK_IDENT
 *   eat(SB_SEMICOLON);  → Lỗi "1-5:Missing ;"
 *
 * Sử dụng:
 *   - Được gọi khi parser biết chính xác token tiếp theo phải là gì
 *   - Ví dụ: sau KW_PROGRAM phải có TK_IDENT, sau đó phải có SB_SEMICOLON
 */
void eat(TokenType tokenType);

/* ================== Các hàm phân tích chương trình ================== */

/**
 * Phân tích toàn bộ chương trình
 *
 * Grammar: Program → PROGRAM Ident ; Block .
 *
 * Ví dụ:
 *   PROGRAM Example1;
 *   BEGIN
 *     ...
 *   END.
 *
 * Luồng xử lý:
 *   1. eat(KW_PROGRAM)
 *   2. eat(TK_IDENT)
 *   3. eat(SB_SEMICOLON)
 *   4. compileBlock()
 *   5. eat(SB_PERIOD)
 *
 * Đây là entry point của parser, được gọi từ hàm compile()
 */
void compileProgram(void);

/**
 * Phân tích một block (khối chương trình)
 *
 * Grammar:
 *   Block → ConstDecls TypeDecls VarDecls SubDecls Statements
 *
 * Cấu trúc:
 *   CONST ...     (optional - compileConstDecls)
 *   TYPE ...      (optional - compileTypeDecls)
 *   VAR ...       (optional - compileVarDecls)
 *   FUNCTION ...  (optional - compileSubDecls)
 *   PROCEDURE ... (optional - compileSubDecls)
 *   BEGIN
 *     ...         (compileStatements)
 *   END
 *
 * Ví dụ:
 *   VAR n : Integer;
 *   FUNCTION F(x : Integer) : Integer;
 *   BEGIN
 *     F := x * 2;
 *   END;
 *   BEGIN
 *     n := 5;
 *   END
 *
 * Lưu ý:
 *   - Block được chia nhỏ thành compileBlock2, 3, 4, 5 để xử lý optional parts
 *   - Mỗi block có thể chứa các block con (trong function/procedure)
 */
void compileBlock(void);

/**
 * Phân tích phần TYPE và phần còn lại của block
 *
 * Grammar: Block2 → TypeDecls VarDecls SubDecls Statements
 *
 * Được gọi sau khi xử lý xong phần CONST
 */
void compileBlock2(void);

/**
 * Phân tích phần VAR và phần còn lại của block
 *
 * Grammar: Block3 → VarDecls SubDecls Statements
 *
 * Được gọi sau khi xử lý xong phần TYPE
 */
void compileBlock3(void);

/**
 * Phân tích phần SubDecls (FUNCTION/PROCEDURE) và phần còn lại
 *
 * Grammar: Block4 → SubDecls Statements
 *
 * Được gọi sau khi xử lý xong phần VAR
 */
void compileBlock4(void);

/**
 * Phân tích phần statements (BEGIN...END)
 *
 * Grammar: Block5 → BEGIN Statements END
 *
 * Đây là phần bắt buộc của mọi block
 */
void compileBlock5(void);

/* ================== Phân tích khai báo ================== */

/**
 * Phân tích danh sách khai báo hằng (CONST)
 *
 * Grammar: ConstDecls → ConstDecl ConstDecl ... (zero or more)
 *
 * Ví dụ:
 *   CONST
 *     PI = 3.14;
 *     MAX = 100;
 *     MIN = -10;
 *
 * Lưu ý: Hàm này lặp cho đến khi không còn TK_IDENT
 */
void compileConstDecls(void);

/**
 * Phân tích một khai báo hằng
 *
 * Grammar: ConstDecl → Ident = Constant ;
 *
 * Ví dụ:
 *   MAX = 100;
 *   PI = 3.14;
 *
 * Luồng:
 *   1. eat(TK_IDENT)      - Tên hằng
 *   2. eat(SB_EQ)         - Dấu '='
 *   3. compileConstant()  - Giá trị hằng
 *   4. eat(SB_SEMICOLON)  - Dấu ';'
 */
void compileConstDecl(void);

/**
 * Phân tích danh sách khai báo kiểu (TYPE)
 *
 * Grammar: TypeDecls → TypeDecl TypeDecl ... (zero or more)
 *
 * Ví dụ:
 *   TYPE
 *     Vector = ARRAY [10] OF Integer;
 *     Matrix = ARRAY [5] OF Vector;
 */
void compileTypeDecls(void);

/**
 * Phân tích một khai báo kiểu
 *
 * Grammar: TypeDecl → Ident = Type ;
 *
 * Ví dụ:
 *   Vector = ARRAY [10] OF Integer;
 *
 * Luồng:
 *   1. eat(TK_IDENT)      - Tên kiểu mới
 *   2. eat(SB_EQ)         - Dấu '='
 *   3. compileType()      - Định nghĩa kiểu
 *   4. eat(SB_SEMICOLON)  - Dấu ';'
 */
void compileTypeDecl(void);

/**
 * Phân tích danh sách khai báo biến (VAR)
 *
 * Grammar: VarDecls → VarDecl VarDecl ... (zero or more)
 *
 * Ví dụ:
 *   VAR
 *     n : Integer;
 *     c : Char;
 *     arr : ARRAY [10] OF Integer;
 */
void compileVarDecls(void);

/**
 * Phân tích một khai báo biến
 *
 * Grammar: VarDecl → Ident : Type ;
 *
 * Ví dụ:
 *   n : Integer;
 *   arr : ARRAY [10] OF Integer;
 *
 * Luồng:
 *   1. eat(TK_IDENT)      - Tên biến
 *   2. eat(SB_COLON)      - Dấu ':'
 *   3. compileType()      - Kiểu dữ liệu
 *   4. eat(SB_SEMICOLON)  - Dấu ';'
 */
void compileVarDecl(void);

/**
 * Phân tích danh sách khai báo hàm/thủ tục
 *
 * Grammar: SubDecls → (FuncDecl | ProcDecl)*
 *
 * Ví dụ:
 *   FUNCTION F(n : Integer) : Integer;
 *   BEGIN
 *     F := n * 2;
 *   END;
 *
 *   PROCEDURE Print(x : Integer);
 *   BEGIN
 *     CALL WriteI(x);
 *   END;
 *
 * Lưu ý:
 *   - Hàm này đệ quy để xử lý nhiều function/procedure
 *   - In assert "Parsing/Subtoutines parsed ...." ở đầu và cuối
 */
void compileSubDecls(void);

/**
 * Phân tích khai báo hàm (FUNCTION)
 *
 * Grammar: FuncDecl → FUNCTION Ident Params : BasicType ; Block ;
 *
 * Ví dụ:
 *   FUNCTION Factorial(n : Integer) : Integer;
 *   BEGIN
 *     IF n = 0 THEN Factorial := 1
 *     ELSE Factorial := n * Factorial(n - 1);
 *   END;
 *
 * Luồng:
 *   1. eat(KW_FUNCTION)
 *   2. eat(TK_IDENT)       - Tên hàm
 *   3. compileParams()     - Danh sách tham số
 *   4. eat(SB_COLON)
 *   5. compileBasicType()  - Kiểu trả về
 *   6. eat(SB_SEMICOLON)
 *   7. compileBlock()      - Thân hàm
 *   8. eat(SB_SEMICOLON)
 *
 * Lưu ý: Hàm phải gán giá trị cho chính tên hàm (Factorial := ...)
 */
void compileFuncDecl(void);

/**
 * Phân tích khai báo thủ tục (PROCEDURE)
 *
 * Grammar: ProcDecl → PROCEDURE Ident Params ; Block ;
 *
 * Ví dụ:
 *   PROCEDURE PrintArray(arr : ARRAY [10] OF Integer);
 *   VAR i : Integer;
 *   BEGIN
 *     FOR i := 0 TO 9 DO
 *       CALL WriteI(arr[i]);
 *   END;
 *
 * Luồng:
 *   1. eat(KW_PROCEDURE)
 *   2. eat(TK_IDENT)       - Tên thủ tục
 *   3. compileParams()     - Danh sách tham số
 *   4. eat(SB_SEMICOLON)
 *   5. compileBlock()      - Thân thủ tục
 *   6. eat(SB_SEMICOLON)
 *
 * Lưu ý: Thủ tục không trả về giá trị (khác với FUNCTION)
 */
void compileProcDecl(void);

/* ================== Phân tích hằng số và kiểu ================== */

/**
 * Phân tích hằng số không dấu
 *
 * Grammar: UnsignedConstant → Number | Char | Ident
 *
 * Ví dụ:
 *   123        - TK_NUMBER
 *   'a'        - TK_CHAR
 *   MAX        - TK_IDENT (hằng đã khai báo trước)
 *
 * Luồng: Switch theo lookAhead và eat token tương ứng
 */
void compileUnsignedConstant(void);

/**
 * Phân tích hằng số (có thể có dấu)
 *
 * Grammar: Constant → (+|-) UnsignedConstant | UnsignedConstant
 *
 * Ví dụ:
 *   +100
 *   -50
 *   'a'
 *   MAX
 *
 * Luồng:
 *   - Nếu có dấu '+' hoặc '-' → eat dấu, sau đó compileConstant2()
 *   - Nếu không → compileConstant2() trực tiếp
 */
void compileConstant(void);

/**
 * Phân tích phần còn lại của hằng số (sau dấu +/-)
 *
 * Grammar: Constant2 → Ident | Number | Char
 *
 * Được gọi từ compileConstant() sau khi xử lý dấu
 */
void compileConstant2(void);

/**
 * Phân tích kiểu dữ liệu
 *
 * Grammar: Type → INTEGER | CHAR | ARRAY [Number] OF Type | Ident
 *
 * Ví dụ:
 *   Integer                        - Kiểu cơ bản
 *   Char                           - Kiểu cơ bản
 *   ARRAY [10] OF Integer          - Kiểu mảng
 *   ARRAY [5] OF ARRAY [3] OF Char - Mảng 2 chiều
 *   Vector                         - Kiểu đã định nghĩa
 *
 * Luồng:
 *   - KW_INTEGER/KW_CHAR → eat token
 *   - KW_ARRAY → eat KW_ARRAY, [, Number, ], OF, compileType() (đệ quy)
 *   - TK_IDENT → eat TK_IDENT (kiểu tự định nghĩa)
 */
void compileType(void);

/**
 * Phân tích kiểu cơ bản (INTEGER | CHAR)
 *
 * Grammar: BasicType → INTEGER | CHAR
 *
 * Ví dụ:
 *   Integer
 *   Char
 *
 * Sử dụng: Kiểu trả về của FUNCTION
 */
void compileBasicType(void);

/* ================== Phân tích tham số ================== */

/**
 * Phân tích danh sách tham số
 *
 * Grammar: Params → ( Param Params2 ) | ε
 *
 * Ví dụ:
 *   ()                           - Không có tham số
 *   (n : Integer)                - 1 tham số
 *   (x : Integer; y : Integer)   - 2 tham số
 *
 * Luồng:
 *   - Nếu có '(' → eat '(', compileParam(), compileParams2(), eat ')'
 *   - Nếu không → epsilon (không làm gì)
 */
void compileParams(void);

/**
 * Phân tích các tham số tiếp theo (sau tham số đầu tiên)
 *
 * Grammar: Params2 → ; Param Params2 | ε
 *
 * Ví dụ:
 *   ; y : Integer; z : Char
 *
 * Lưu ý: Đệ quy để xử lý nhiều tham số
 */
void compileParams2(void);

/**
 * Phân tích một tham số
 *
 * Grammar: Param → Ident : Type
 *
 * Ví dụ:
 *   n : Integer
 *   arr : ARRAY [10] OF Integer
 *
 * Luồng:
 *   1. eat(TK_IDENT)  - Tên tham số
 *   2. eat(SB_COLON)  - Dấu ':'
 *   3. compileType()  - Kiểu dữ liệu
 */
void compileParam(void);

/* ================== Phân tích câu lệnh ================== */

/**
 * Phân tích danh sách câu lệnh
 *
 * Grammar: Statements → Statement Statements2
 *
 * Ví dụ:
 *   n := 5;
 *   CALL WriteI(n);
 *   IF n > 0 THEN n := n - 1;
 *
 * Luồng:
 *   1. compileStatement()   - Câu lệnh đầu tiên
 *   2. compileStatements2() - Các câu lệnh tiếp theo (nếu có)
 */
void compileStatements(void);

/**
 * Phân tích các câu lệnh tiếp theo (phân cách bởi ';')
 *
 * Grammar: Statements2 → ; Statement Statements2 | ε
 *
 * Ví dụ:
 *   ; CALL WriteI(x); n := n + 1;
 *
 * Luồng:
 *   - Nếu có ';' → eat ';', compileStatement(), compileStatements2() (đệ quy)
 *   - Nếu không → epsilon (kết thúc)
 *
 * FOLLOW set: END, ELSE
 */
void compileStatements2(void);

/**
 * Phân tích một câu lệnh
 *
 * Grammar: Statement → AssignSt | CallSt | GroupSt | IfSt | WhileSt | ForSt | ε
 *
 * Các loại câu lệnh:
 *   - Ident ...     → AssignSt (n := 5)
 *   - CALL ...      → CallSt
 *   - BEGIN...END   → GroupSt
 *   - IF...THEN     → IfSt
 *   - WHILE...DO    → WhileSt
 *   - FOR...TO...DO → ForSt
 *   - ε             → Empty statement
 *
 * Luồng: Switch theo lookAhead token
 */
void compileStatement(void);

/**
 * Phân tích câu lệnh gán
 *
 * Grammar: AssignSt → Ident Indexes := Expression
 *
 * Ví dụ:
 *   n := 5;
 *   arr[i] := 10;
 *   matrix[i][j] := 0;
 *
 * Luồng:
 *   1. eat(TK_IDENT)       - Tên biến
 *   2. compileIndexes()    - Chỉ số mảng (nếu có)
 *   3. eat(SB_ASSIGN)      - Dấu ':='
 *   4. compileExpression() - Biểu thức bên phải
 */
void compileAssignSt(void);

/**
 * Phân tích câu lệnh gọi thủ tục/hàm
 *
 * Grammar: CallSt → CALL Ident Arguments
 *
 * Ví dụ:
 *   CALL WriteI(123);
 *   CALL PrintArray(arr);
 *   CALL Swap(x, y);
 *
 * Luồng:
 *   1. eat(KW_CALL)
 *   2. eat(TK_IDENT)       - Tên thủ tục/hàm
 *   3. compileArguments()  - Danh sách đối số
 */
void compileCallSt(void);

/**
 * Phân tích khối câu lệnh (BEGIN...END)
 *
 * Grammar: GroupSt → BEGIN Statements END
 *
 * Ví dụ:
 *   BEGIN
 *     n := 5;
 *     CALL WriteI(n);
 *   END
 *
 * Luồng:
 *   1. eat(KW_BEGIN)
 *   2. compileStatements()
 *   3. eat(KW_END)
 */
void compileGroupSt(void);

/**
 * Phân tích câu lệnh IF
 *
 * Grammar: IfSt → IF Condition THEN Statement ElseSt
 *
 * Ví dụ:
 *   IF n > 0 THEN n := n - 1;
 *
 *   IF x = 0 THEN
 *     result := 1
 *   ELSE
 *     result := x * Factorial(x - 1);
 *
 * Luồng:
 *   1. eat(KW_IF)
 *   2. compileCondition()  - Điều kiện
 *   3. eat(KW_THEN)
 *   4. compileStatement()  - Câu lệnh nếu đúng
 *   5. compileElseSt()     - Câu lệnh ELSE (nếu có)
 */
void compileIfSt(void);

/**
 * Phân tích phần ELSE (optional)
 *
 * Grammar: ElseSt → ELSE Statement | ε
 *
 * Ví dụ:
 *   ELSE result := 0;
 *
 * Luồng:
 *   - Nếu có ELSE → eat(KW_ELSE), compileStatement()
 *   - Nếu không → epsilon
 */
void compileElseSt(void);

/**
 * Phân tích câu lệnh WHILE
 *
 * Grammar: WhileSt → WHILE Condition DO Statement
 *
 * Ví dụ:
 *   WHILE n > 0 DO
 *     n := n - 1;
 *
 *   WHILE i < 10 DO BEGIN
 *     CALL WriteI(arr[i]);
 *     i := i + 1;
 *   END;
 *
 * Luồng:
 *   1. eat(KW_WHILE)
 *   2. compileCondition()  - Điều kiện lặp
 *   3. eat(KW_DO)
 *   4. compileStatement()  - Thân vòng lặp
 */
void compileWhileSt(void);

/**
 * Phân tích câu lệnh FOR
 *
 * Grammar: ForSt → FOR Ident := Expression TO Expression DO Statement
 *
 * Ví dụ:
 *   FOR i := 1 TO 10 DO
 *     CALL WriteI(i);
 *
 *   FOR n := 0 TO 5 DO BEGIN
 *     result := result * n;
 *   END;
 *
 * Luồng:
 *   1. eat(KW_FOR)
 *   2. eat(TK_IDENT)        - Biến đếm
 *   3. eat(SB_ASSIGN)       - ':='
 *   4. compileExpression()  - Giá trị bắt đầu
 *   5. eat(KW_TO)
 *   6. compileExpression()  - Giá trị kết thúc
 *   7. eat(KW_DO)
 *   8. compileStatement()   - Thân vòng lặp
 *
 * Lưu ý: Biến đếm luôn tăng dần (không có DOWNTO)
 */
void compileForSt(void);

/* ================== Phân tích đối số ================== */

/**
 * Phân tích danh sách đối số
 *
 * Grammar: Arguments → ( Expression Arguments2 ) | ε
 *
 * Ví dụ:
 *   ()                    - Không có đối số
 *   (123)                 - 1 đối số
 *   (x + 5, arr[i], 'a')  - 3 đối số
 *
 * Luồng:
 *   - Nếu có '(' → eat '(', compileExpression(), compileArguments2(), eat ')'
 *   - Nếu không → epsilon
 *
 * FOLLOW set rất lớn (bao gồm semicolon, operators, keywords...)
 */
void compileArguments(void);

/**
 * Phân tích các đối số tiếp theo (sau đối số đầu tiên)
 *
 * Grammar: Arguments2 → , Expression Arguments2 | ε
 *
 * Ví dụ:
 *   , y + 1, arr[i]
 *
 * Luồng:
 *   - Nếu có ',' → eat ',', compileExpression(), compileArguments2() (đệ quy)
 *   - Nếu không → epsilon
 *
 * FOLLOW set: ')'
 */
void compileArguments2(void);

/* ================== Phân tích biểu thức ================== */

/**
 * Phân tích biểu thức điều kiện
 *
 * Grammar: Condition → Expression Condition2
 *
 * Ví dụ:
 *   n > 0
 *   x = y
 *   a + b <= c * d
 *
 * Luồng:
 *   1. compileExpression()  - Vế trái
 *   2. compileCondition2()  - Toán tử so sánh + vế phải
 */
void compileCondition(void);

/**
 * Phân tích toán tử so sánh và vế phải
 *
 * Grammar: Condition2 → (= | != | < | <= | > | >=) Expression
 *
 * Ví dụ:
 *   = 0
 *   > 5
 *   <= MAX
 *
 * Luồng:
 *   1. Switch theo lookAhead (SB_EQ, SB_NEQ, SB_LT, SB_LE, SB_GT, SB_GE)
 *   2. eat toán tử
 *   3. compileExpression() - Vế phải
 */
void compileCondition2(void);

/**
 * Phân tích biểu thức (có thể có dấu +/- ở đầu)
 *
 * Grammar: Expression → (+|-) Expression2 | Expression2
 *
 * Ví dụ:
 *   +x
 *   -5
 *   a + b * c
 *
 * Luồng:
 *   - Nếu có dấu '+' hoặc '-' → eat dấu, compileExpression2()
 *   - Nếu không → compileExpression2() trực tiếp
 */
void compileExpression(void);

/**
 * Phân tích term đầu tiên và các term tiếp theo
 *
 * Grammar: Expression2 → Term Expression3
 *
 * Ví dụ:
 *   5 + 10 - 3
 *
 * Luồng:
 *   1. compileTerm()        - Term đầu tiên
 *   2. compileExpression3() - Các term tiếp theo (nếu có)
 */
void compileExpression2(void);

/**
 * Phân tích các term tiếp theo (phép cộng/trừ)
 *
 * Grammar: Expression3 → (+|-) Term Expression3 | ε
 *
 * Ví dụ:
 *   + 10 - 3
 *
 * Luồng:
 *   - Nếu có '+' hoặc '-' → eat toán tử, compileTerm(), compileExpression3() (đệ quy)
 *   - Nếu không → epsilon
 *
 * FOLLOW set: TO, DO, ), ,, =, !=, <, <=, >, >=, ], ;, END, ELSE, THEN
 */
void compileExpression3(void);

/**
 * Phân tích term (phép nhân/chia)
 *
 * Grammar: Term → Factor Term2
 *
 * Ví dụ:
 *   5 * 10 / 2
 *
 * Luồng:
 *   1. compileFactor() - Factor đầu tiên
 *   2. compileTerm2()  - Các factor tiếp theo (nếu có)
 */
void compileTerm(void);

/**
 * Phân tích các factor tiếp theo (phép nhân/chia)
 *
 * Grammar: Term2 → (*|/) Factor Term2 | ε
 *
 * Ví dụ:
 *   * 10 / 2
 *
 * Luồng:
 *   - Nếu có '*' hoặc '/' → eat toán tử, compileFactor(), compileTerm2() (đệ quy)
 *   - Nếu không → epsilon
 *
 * FOLLOW set: +, -, TO, DO, ), ,, =, !=, <, <=, >, >=, ], ;, END, ELSE, THEN
 */
void compileTerm2(void);

/**
 * Phân tích factor (đơn vị cơ bản trong biểu thức)
 *
 * Grammar: Factor → Number | Char | Ident Indexes Arguments | ( Expression )
 *
 * Ví dụ:
 *   123                  - TK_NUMBER
 *   'a'                  - TK_CHAR
 *   n                    - TK_IDENT (biến)
 *   arr[i]               - TK_IDENT + Indexes (phần tử mảng)
 *   Factorial(5)         - TK_IDENT + Arguments (gọi hàm)
 *   (a + b) * c          - ( Expression )
 *
 * Luồng:
 *   - TK_NUMBER → eat(TK_NUMBER)
 *   - TK_CHAR → eat(TK_CHAR)
 *   - TK_IDENT → eat(TK_IDENT), compileIndexes(), compileArguments()
 *   - SB_LPAR → eat '(', compileExpression(), eat ')'
 */
void compileFactor(void);

/**
 * Phân tích chỉ số mảng (có thể nhiều chiều)
 *
 * Grammar: Indexes → [ Expression ] Indexes | ε
 *
 * Ví dụ:
 *   [5]           - Mảng 1 chiều
 *   [i][j]        - Mảng 2 chiều
 *   [i][j][k]     - Mảng 3 chiều
 *   (không có)    - Biến đơn
 *
 * Luồng:
 *   - While có '[' → eat '[', compileExpression(), eat ']'
 *   - Nếu không → epsilon
 */
void compileIndexes(void);

/* ================== Hàm khởi tạo compiler ================== */

/**
 * Biên dịch file nguồn KPL
 *
 * @param fileName Đường dẫn tới file nguồn (.kpl)
 * @return IO_SUCCESS nếu biên dịch thành công
 *         IO_ERROR nếu không mở được file
 *
 * Luồng hoạt động:
 *   1. openInputStream(fileName)  - Mở file
 *   2. Khởi tạo currentToken = NULL
 *   3. Lấy lookAhead = getValidToken()
 *   4. compileProgram()           - Bắt đầu phân tích
 *   5. Giải phóng token
 *   6. closeInputStream()         - Đóng file
 *
 * Ví dụ sử dụng:
 *   if (compile("example1.kpl") == IO_ERROR) {
 *     printf("Cannot open file!\n");
 *   }
 *
 * Lưu ý:
 *   - Nếu gặp lỗi cú pháp, chương trình sẽ exit(1) trong hàm error()
 *   - File phải có định dạng đúng ngữ pháp KPL
 */
int compile(char *fileName);

#endif
