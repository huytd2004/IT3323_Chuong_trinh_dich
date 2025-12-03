/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#include "token.h"

/*
 * Enum liệt kê các mã lỗi (ErrorCode) có thể gặp trong quá trình
 * phân tích từ vựng, cú pháp và kiểm tra ngữ nghĩa của chương trình.
 * Mỗi lỗi tương ứng với một tình huống sai sót cụ thể.
 */
typedef enum
{
  ERR_ENDOFCOMMENT,        // Lỗi: kết thúc comment (*/) không tìm thấy
  ERR_IDENTTOOLONG,        // Lỗi: tên định danh quá dài
  ERR_INVALIDCHARCONSTANT, // Lỗi: ký tự hằng không hợp lệ
  ERR_INVALIDSYMBOL,       // Lỗi: ký tự/symbol không hợp lệ
  ERR_INVALIDCONSTANT,     // Lỗi: hằng số không hợp lệ
  ERR_INVALIDTYPE,         // Lỗi: kiểu dữ liệu không hợp lệ
  ERR_INVALIDBASICTYPE,    // Lỗi: kiểu dữ liệu cơ bản không hợp lệ
  ERR_INVALIDPARAM,        // Lỗi: tham số không hợp lệ
  ERR_INVALIDSTATEMENT,    // Lỗi: câu lệnh không hợp lệ
  ERR_INVALIDARGUMENTS,    // Lỗi: đối số không hợp lệ
  ERR_INVALIDCOMPARATOR,   // Lỗi: toán tử so sánh không hợp lệ
  ERR_INVALIDEXPRESSION,   // Lỗi: biểu thức không hợp lệ
  ERR_INVALIDTERM,         // Lỗi: term trong biểu thức không hợp lệ
  ERR_INVALIDFACTOR,       // Lỗi: factor trong biểu thức không hợp lệ
  ERR_INVALIDCONSTDECL,    // Lỗi: khai báo hằng số không hợp lệ
  ERR_INVALIDTYPEDECL,     // Lỗi: khai báo kiểu không hợp lệ
  ERR_INVALIDVARDECL,      // Lỗi: khai báo biến không hợp lệ
  ERR_INVALIDSUBDECL,      // Lỗi: khai báo thủ tục/hàm không hợp lệ
} ErrorCode;

/*
 * Các chuỗi thông báo lỗi tương ứng với mỗi ErrorCode
 * Dùng để hiển thị thông báo cho người dùng hoặc ghi log.
 */
#define ERM_ENDOFCOMMENT "End of comment expected!"
#define ERM_IDENTTOOLONG "Identification too long!"
#define ERM_INVALIDCHARCONSTANT "Invalid const char!"
#define ERM_INVALIDSYMBOL "Invalid symbol!"
#define ERM_INVALIDCONSTANT "Invalid constant!"
#define ERM_INVALIDTYPE "Invalid type!"
#define ERM_INVALIDBASICTYPE "Invalid basic type!"
#define ERM_INVALIDPARAM "Invalid parameter!"
#define ERM_INVALIDSTATEMENT "Invalid statement!"
#define ERM_INVALIDARGUMENTS "Invalid arguments!"
#define ERM_INVALIDCOMPARATOR "Invalid comparator!"
#define ERM_INVALIDEXPRESSION "Invalid expression!"
#define ERM_INVALIDTERM "Invalid term!"
#define ERM_INVALIDFACTOR "Invalid factor!"
#define ERM_INVALIDCONSTDECL "Invalid constant declaration!"
#define ERM_INVALIDTYPEDECL "Invalid type declaration!"
#define ERM_INVALIDVARDECL "Invalid variable declaration!"
#define ERM_INVALIDSUBDECL "Invalid subroutine declaration!"

/*
 * Hàm xử lý lỗi:
 *  - error(): in thông báo lỗi tại dòng và cột xác định
 *  - missingToken(): thông báo khi thiếu một token nhất định
 *  - assert(): kiểm tra điều kiện, nếu sai in ra thông báo
 */
void error(ErrorCode err, int lineNo, int colNo);
void missingToken(TokenType tokenType, int lineNo, int colNo);
void assert(char *msg);

#endif
