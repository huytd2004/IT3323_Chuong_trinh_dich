/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab *symtab;
extern Token *currentToken;

/**
 * Tìm kiếm một object theo tên trong bảng ký hiệu
 * Tìm từ scope hiện tại ra ngoài, sau đó tìm trong danh sách global
 * @param name: Tên của object cần tìm
 * @return: Con trỏ tới object nếu tìm thấy, NULL nếu không tìm thấy
 */
Object *lookupObject(char *name)
{
  // Tìm kiếm từ scope hiện tại ra ngoài, sau đó trong danh sách global
  Scope *scope = symtab->currentScope;
  Object *obj;

  // Duyệt qua các scope từ trong ra ngoài
  while (scope != NULL)
  {
    obj = findObject(scope->objList, name);
    if (obj != NULL)
      return obj;         // Tìm thấy trong scope hiện tại
    scope = scope->outer; // Chuyển sang scope bên ngoài
  }

  // Cuối cùng, tìm trong danh sách global objects (built-ins, program, etc.)
  obj = findObject(symtab->globalObjectList, name);
  if (obj != NULL)
    return obj;
  return NULL; // Không tìm thấy
}

/**
 * Kiểm tra identifier có phải là mới (chưa được khai báo trong scope hiện tại)
 * @param name: Tên identifier cần kiểm tra
 * Báo lỗi nếu identifier đã tồn tại trong scope hiện tại
 */
void checkFreshIdent(char *name)
{
  // Đảm bảo identifier chưa được khai báo trong scope hiện tại
  if (findObject(symtab->currentScope->objList, name) != NULL)
  {
    // Báo lỗi trùng tên
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
  }
}

/**
 * Kiểm tra identifier đã được khai báo chưa
 * @param name: Tên identifier cần kiểm tra
 * @return: Object tương ứng nếu đã được khai báo
 * Báo lỗi nếu chưa được khai báo
 */
Object *checkDeclaredIdent(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  return obj;
}

/**
 * Kiểm tra constant đã được khai báo chưa
 * @param name: Tên constant cần kiểm tra
 * @return: Object constant tương ứng
 * Báo lỗi nếu chưa khai báo hoặc không phải constant
 */
Object *checkDeclaredConstant(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL || obj->kind != OBJ_CONSTANT)
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  return obj;
}

/**
 * Kiểm tra type đã được khai báo chưa
 * @param name: Tên type cần kiểm tra
 * @return: Object type tương ứng
 * Báo lỗi nếu chưa khai báo hoặc không phải type
 */
Object *checkDeclaredType(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL || obj->kind != OBJ_TYPE)
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  return obj;
}

/**
 * Kiểm tra variable đã được khai báo chưa
 * @param name: Tên variable cần kiểm tra
 * @return: Object variable tương ứng
 * Báo lỗi nếu chưa khai báo hoặc không phải variable
 */
Object *checkDeclaredVariable(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL || obj->kind != OBJ_VARIABLE)
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  return obj;
}

/**
 * Kiểm tra function đã được khai báo chưa
 * @param name: Tên function cần kiểm tra
 * @return: Object function tương ứng
 * Báo lỗi nếu chưa khai báo hoặc không phải function
 */
Object *checkDeclaredFunction(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL || obj->kind != OBJ_FUNCTION)
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  return obj;
}

/**
 * Kiểm tra procedure đã được khai báo chưa
 * @param name: Tên procedure cần kiểm tra
 * @return: Object procedure tương ứng
 * Báo lỗi nếu chưa khai báo hoặc không phải procedure
 */
Object *checkDeclaredProcedure(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL || obj->kind != OBJ_PROCEDURE)
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  return obj;
}

/**
 * Kiểm tra lvalue identifier đã được khai báo và có thể gán giá trị
 * LValue là giá trị có thể xuất hiện ở vế trái của phép gán
 * @param name: Tên identifier cần kiểm tra
 * @return: Object tương ứng nếu hợp lệ
 * Báo lỗi nếu chưa khai báo hoặc không phải lvalue hợp lệ
 */
Object *checkDeclaredLValueIdent(char *name)
{
  Object *obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

  // Cho phép variable hoặc parameter (có thể gán giá trị)
  if (obj->kind == OBJ_VARIABLE || obj->kind == OBJ_PARAMETER)
    return obj;

  // Cho phép gán giá trị cho tên function bên trong thân function đó
  // (để trả về giá trị của function)
  if (obj->kind == OBJ_FUNCTION && symtab->currentScope->owner == obj)
    return obj;

  // Các trường hợp khác không phải lvalue hợp lệ
  error(ERR_INVALID_LVALUE, currentToken->lineNo, currentToken->colNo);
  return obj; // không thể đến được, nhưng giữ cho compiler không cảnh báo
}
