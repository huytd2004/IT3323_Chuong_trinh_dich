huytd2004: Tóm tắt codebase

Workspace: Collecting workspace information# Tóm tắt Codebase - Compiler Lab 1

Đây là một dự án **lexical scanner (bộ phân tích từ vựng)** cho một ngôn ngữ lập trình giáo dục có tên **KPL (KPL Programming Language)**.

## Cấu trúc Dự án

### Thư mục `incompleted/`
Chứa mã nguồn chính của scanner:

**Các file header:**
- `charcode.h` - Định nghĩa enum `CharCode` để phân loại ký tự (chữ cái, số, toán tử, v.v.)
- `reader.h` - Interface để đọc file đầu vào
- `token.h` - Định nghĩa enum `TokenType` và struct `Token`
- `error.h` - Định nghĩa các mã lỗi và thông báo lỗi

**Các file implementation:**
- `charcode.c` - Mảng `charCodes[256]` ánh xạ ký tự ASCII sang `CharCode`
- `reader.c` - Đọc từng ký tự từ file, theo dõi `lineNo`, `colNo`
- `token.c` - Chứa danh sách 20 keywords và hàm `checkKeyword()`
- `error.c` - Hàm `error()` để in lỗi và thoát chương trình
- `scanner.c` - **File chính cần hoàn thiện**, chứa các hàm TODO:
  - `skipBlank()` - Bỏ qua khoảng trắng
  - `skipComment()` - Bỏ qua comment `(* ... *)`
  - `readIdentKeyword()` - Đọc identifier hoặc keyword
  - `readNumber()` - Đọc số
  - `readConstChar()` - Đọc ký tự trong dấu nháy đơn `'c'`
  - `getToken()` - Hàm chính trả về token tiếp theo

**File khác:**
- `main.c` - Entry point để chạy scanner
- `unit_test_scanner.c` - Unit tests sử dụng thư viện cmocka
- `Makefile` - Build script

### Thư mục `test/`
Chứa 7 file test case KPL và kết quả mong đợi:
- `example1.kpl` - `result1.txt`: Chương trình đơn giản nhất
- `example2.kpl` - `result2.txt`: Hàm đệ quy tính giai thừa
- `example3.kpl` - `result3.txt`: Thuật toán Tháp Hà Nội (đúng)
- `example4.kpl` - `result4.txt`: Lỗi `!` và `=` tách rời thay vì `!=`
- `example5.kpl` - `result5.txt`: Lỗi comment không đóng
- `example6.kpl` - `result6.txt`: Lỗi char constant `'abc'` (nhiều hơn 1 ký tự)
- `example7.kpl` - `result7.txt`: Lỗi char constant `"abc"` (sử dụng dấu nháy kép)

## Ngôn ngữ KPL

**Keywords (20):** `PROGRAM`, `CONST`, `TYPE`, `VAR`, `INTEGER`, `CHAR`, `ARRAY`, `OF`, `FUNCTION`, `PROCEDURE`, `BEGIN`, `END`, `CALL`, `IF`, `THEN`, `ELSE`, `WHILE`, `DO`, `FOR`, `TO`

**Symbols:** `;` `:` `.` `,` `:=` `=` `!=` `<` `<=` `>` `>=` `+` `-` `*` `/` `(` `)` `[` `]`

**Comments:** `(* ... *)` (có thể lồng nhau)

**Tokens:**
- Identifiers: chữ cái + (chữ cái|số)*, tối đa 15 ký tự
- Numbers: chuỗi chữ số
- Char constants: `'c'` (1 ký tự trong dấu nháy đơn)

## Nhiệm vụ

Hoàn thiện các hàm TODO trong `scanner.c` để scanner có thể:
1. Bỏ qua khoảng trắng và comments
2. Nhận diện keywords, identifiers, numbers, char constants
3. Nhận diện các symbols và operators
4. Báo lỗi phù hợp với các trường hợp trong test cases

huytd2004: Luồng chạy của codebase

GitHub Copilot: # Luồng Chạy của Codebase

## 1. Khởi động chương trình

```
main.c: main() 
  ↓
Kiểm tra argument (file đầu vào)
  ↓
Gọi scan(argv[1])
```

## 2. Hàm `scan()` trong scanner.c

```c
scan(filename)
  ↓
openInputStream(filename)  // Mở file KPL
  ↓
printToken(getToken())     // Lặp lại cho đến hết file
  ↓
printToken(getToken())
  ↓
...
  ↓
closeInputStream()         // Đóng file
```

## 3. Hàm `getToken()` - Trái tim của Scanner

```
getToken()
  ↓
┌─────────────────────────────────────┐
│ Bước 1: Bỏ qua whitespace          │
│   skipBlank()                       │
│   - Đọc và bỏ qua space, tab, \n   │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│ Bước 2: Kiểm tra ký tự tiếp theo   │
│   charCode = getCharCode(readChar())│
└─────────────────────────────────────┘
  ↓
┌──────────────────────────────────────────────────────────┐
│ Bước 3: Phân tích theo loại ký tự                       │
└──────────────────────────────────────────────────────────┘
  ↓
  ├── CHAR_LETTER → readIdentKeyword()
  │                  ↓
  │                  Đọc chuỗi chữ cái + số
  │                  ↓
  │                  checkKeyword() 
  │                  ↓
  │                  ├─ Keyword? → TK_xxx
  │                  └─ Không?    → TK_IDENT
  │
  ├── CHAR_DIGIT → readNumber()
  │                 ↓
  │                 Đọc chuỗi chữ số
  │                 ↓
  │                 return TK_NUMBER
  │
  ├── CHAR_SINGLEQUOTE → readConstChar()
  │                       ↓
  │                       Đọc 'c'
  │                       ↓
  │                       return TK_CHAR
  │
  ├── CHAR_PLUS → return makeToken(TK_PLUS, ...)
  ├── CHAR_MINUS → return makeToken(TK_MINUS, ...)
  ├── CHAR_TIMES → return makeToken(TK_TIMES, ...)
  ├── CHAR_SLASH → return makeToken(TK_SLASH, ...)
  │
  ├── CHAR_LT → readChar()
  │             ↓
  │             ├─ '=' → TK_LE  (<=)
  │             └─ khác → TK_LT (<)
  │
  ├── CHAR_GT → readChar()
  │             ↓
  │             ├─ '=' → TK_GE  (>=)
  │             └─ khác → TK_GT (>)
  │
  ├── CHAR_EXCLAIMATION → readChar()
  │                       ↓
  │                       ├─ '=' → TK_NE  (!=)
  │                       └─ khác → error("Invalid symbol !")
  │
  ├── CHAR_EQ → return makeToken(TK_EQ, ...)
  │
  ├── CHAR_COMMA → return makeToken(TK_COMMA, ...)
  ├── CHAR_PERIOD → readChar()
  │                 ↓
  │                 ├─ ')' → TK_RSEL  (.) array selector
  │                 └─ khác → TK_PERIOD
  │
  ├── CHAR_COLON → readChar()
  │                ↓
  │                ├─ '=' → TK_ASSIGN  (:=)
  │                └─ khác → TK_COLON   (:)
  │
  ├── CHAR_SEMICOLON → return makeToken(TK_SEMICOLON, ...)
  │
  ├── CHAR_LPAR → readChar()  // (
  │               ↓
  │               ├─ '*' → skipComment()  // (* comment *)
  │               │         ↓
  │               │         getToken() (đệ quy)
  │               ├─ '.' → TK_LSEL  (.( array selector
  │               └─ khác → TK_LPAR
  │
  ├── CHAR_RPAR → return makeToken(TK_RPAR, ...)  // )
  ├── CHAR_LBRACKET → return makeToken(TK_LBRACKET, ...)  // [
  ├── CHAR_RBRACKET → return makeToken(TK_RBRACKET, ...)  // ]
  │
  └── default → error("Invalid symbol")
```

## 4. Các Hàm Hỗ Trợ từ reader.c

```
readChar()
  ↓
Đọc 1 ký tự từ file
  ↓
Cập nhật lineNo, colNo
  ↓
Trả về ký tự hoặc EOF

────────────────────

getCharCode(ch)
  ↓
return charCodes[ch]
  ↓
Ánh xạ ASCII → CharCode enum
```

## 5. Xử Lý Comment (lồng nhau)

```
skipComment()
  ↓
┌────────────────────────────┐
│ level = 1                  │
│ while (level > 0)          │
└────────────────────────────┘
  ↓
  readChar()
  ↓
  ├─ '(' → readChar()
  │         ↓
  │         '*' → level++  // Comment lồng
  │
  ├─ '*' → readChar()
  │         ↓
  │         ')' → level--  // Kết thúc 1 level
  │
  ├─ EOF → error("End-of-comment expected!")
  │
  └─ khác → tiếp tục đọc
```

## 6. Ví dụ Cụ Thể

### Input: `example1.kpl`
```kpl
PROGRAM demo;
BEGIN
  CALL WriteI(123);
END.
```

### Luồng thực thi:

```
getToken() #1
  → skipBlank()
  → đọc 'P' → CHAR_LETTER
  → readIdentKeyword() → "PROGRAM"
  → checkKeyword("PROGRAM") → TK_PROGRAM
  → return Token(TK_PROGRAM, "PROGRAM", line=1, col=1)

getToken() #2
  → skipBlank() (bỏ qua space)
  → đọc 'd' → CHAR_LETTER
  → readIdentKeyword() → "demo"
  → checkKeyword("demo") → NULL (không phải keyword)
  → return Token(TK_IDENT, "demo", line=1, col=9)

getToken() #3
  → skipBlank()
  → đọc ';' → CHAR_SEMICOLON
  → return Token(TK_SEMICOLON, ";", line=1, col=13)

getToken() #4
  → skipBlank() (bỏ qua \n)
  → đọc 'B' → CHAR_LETTER
  → readIdentKeyword() → "BEGIN"
  → checkKeyword("BEGIN") → TK_BEGIN
  → return Token(TK_BEGIN, "BEGIN", line=2, col=1)

... tiếp tục cho đến EOF
```

## 7. Xử Lý Lỗi

```
Khi gặp lỗi trong scanner:
  ↓
error(message)
  ↓
printf("Error at line %d, col %d: %s\n", ...)
  ↓
exit(1)  // Dừng chương trình
```

## Tóm tắt

**Luồng chính:** `main()` → `scan()` → vòng lặp `getToken()` → `printToken()` → kết thúc

**Mỗi lần `getToken()`:** Bỏ qua whitespace/comment → Đọc ký tự → Phân loại → Xử lý theo loại → Trả về Token