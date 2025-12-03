/*
 * scanner.h - Header file cho module Scanner
 * Scanner chịu trách nhiệm lấy các token từ input stream
 * và chuyển chúng cho parser sử dụng.
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __SCANNER_H__ // Include guard để tránh include nhiều lần
#define __SCANNER_H__

#include "token.h" // Scanner sẽ trả về các token, nên cần định nghĩa Token

/**
 * Lấy token tiếp theo từ input stream
 *
 * Hàm này đọc ký tự tiếp theo và tạo token tương ứng.
 * Có thể trả về:
 * - Token hợp lệ (KW_xxx, TK_xxx, SB_xxx)
 * - Token lỗi (TK_NONE) nếu gặp ký tự không hợp lệ
 *
 * @return Con trỏ Token mới được tạo (cần free() sau khi dùng xong)
 *
 * Ví dụ:
 *   Token *t = getToken();  // Đọc "PROGRAM"
 *   // t->tokenType = KW_PROGRAM
 *   // t->string = "PROGRAM"
 *   // t->lineNo = 1, t->colNo = 1
 *   free(t);
 *
 * Lưu ý:
 * - Hàm này KHÔNG bỏ qua whitespace/comment
 * - Parser thường dùng getValidToken() thay vì hàm này
 */
Token *getToken(void);

/**
 * Lấy token hợp lệ tiếp theo (bỏ qua whitespace, comment)
 *
 * Hàm này gọi getToken() liên tục cho đến khi:
 * - Gặp token hợp lệ (không phải TK_NONE)
 * - Hoặc gặp EOF
 *
 * @return Con trỏ Token hợp lệ (cần free() sau khi dùng xong)
 *
 * Ví dụ với input: "PROGRAM   (* comment *)  demo;"
 *   Token *t1 = getValidToken();  // t1 = KW_PROGRAM
 *   Token *t2 = getValidToken();  // t2 = TK_IDENT("demo") - đã bỏ qua space & comment
 *   Token *t3 = getValidToken();  // t3 = SB_SEMICOLON
 *
 * Sử dụng:
 *   - Parser dùng hàm này để lấy lookAhead token
 *   - Đảm bảo chỉ nhận token có ý nghĩa (không phải whitespace/comment)
 *
 * Luồng hoạt động:
 *   1. Gọi getToken()
 *   2. Nếu token->tokenType == TK_NONE → gọi lại getToken()
 *   3. Lặp lại cho đến khi gặp token hợp lệ hoặc EOF
 *   4. Trả về token hợp lệ
 */
Token *getValidToken(void);

/**
 * In thông tin token ra màn hình (dùng để debug/test)
 *
 * Format output: <lineNo>-<colNo>:<tokenType>[(string)]
 *
 * @param token Con trỏ tới token cần in
 *
 * Ví dụ output:
 *   printToken(KW_PROGRAM token)     → "1-1:KW_PROGRAM"
 *   printToken(TK_IDENT("demo"))     → "1-9:TK_IDENT(demo)"
 *   printToken(TK_NUMBER("123"))     → "2-5:TK_NUMBER(123)"
 *   printToken(SB_SEMICOLON)         → "1-13:SB_SEMICOLON"
 *   printToken(TK_EOF)               → "10-1:TK_EOF"
 *
 * Sử dụng:
 *   - Scanner test: in tất cả token từ file
 *   - Parser debug: in token đang xử lý
 *   - Kiểm tra output với expected result
 *
 * Lưu ý:
 *   - Chỉ TK_IDENT, TK_NUMBER, TK_CHAR có in thêm string
 *   - Các token khác chỉ in tokenType
 */
void printToken(Token *token);

#endif
