/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "error.h"

void freeObject(Object *obj);
void freeScope(Scope *scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab *symtab;
Type *intType;
Type *charType;

/******************* Type utilities ******************************/

/**
 * Hàm makeIntType: Tạo kiểu dữ liệu Integer
 * @return: Con trỏ đến Type có typeClass = TP_INT
 *
 * Cấp phát bộ nhớ cho struct Type và đánh dấu là kiểu INT
 */
Type *makeIntType(void)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

/**
 * Hàm makeCharType: Tạo kiểu dữ liệu Character
 * @return: Con trỏ đến Type có typeClass = TP_CHAR
 *
 * Cấp phát bộ nhớ cho struct Type và đánh dấu là kiểu CHAR
 */
Type *makeCharType(void)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

/**
 * Hàm makeArrayType: Tạo kiểu dữ liệu Array
 * @param arraySize: Kích thước mảng (số phần tử)
 * @param elementType: Kiểu dữ liệu của phần tử trong mảng
 * @return: Con trỏ đến Type có typeClass = TP_ARRAY
 *
 * Ví dụ: ARRAY [10] OF INTEGER
 *   - arraySize = 10
 *   - elementType = IntType
 */
Type *makeArrayType(int arraySize, Type *elementType)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

/**
 * Hàm duplicateType: Sao chép (nhân bản) một kiểu dữ liệu
 * @param type: Kiểu dữ liệu cần sao chép
 * @return: Bản sao mới của kiểu dữ liệu
 *
 * Tạo bản sao độc lập:
 * - Nếu là kiểu cơ bản (INT, CHAR): chỉ copy typeClass
 * - Nếu là ARRAY: copy đệ quy cả elementType (để tránh con trỏ trùng)
 */
Type *duplicateType(Type *type)
{
  Type *resultType = (Type *)malloc(sizeof(Type));
  resultType->typeClass = type->typeClass;
  if (type->typeClass == TP_ARRAY)
  {
    resultType->arraySize = type->arraySize;
    resultType->elementType = duplicateType(type->elementType);
  }
  return resultType;
}

/**
 * Hàm compareType: So sánh hai kiểu dữ liệu có giống nhau không
 * @param type1: Kiểu thứ nhất
 * @param type2: Kiểu thứ hai
 * @return: 1 nếu giống nhau, 0 nếu khác nhau
 *
 * Quy tắc so sánh:
 * - INT so với INT: giống
 * - CHAR so với CHAR: giống
 * - ARRAY so với ARRAY: kiểm tra arraySize và elementType (đệ quy)
 * - Các trường hợp khác: khác nhau
 */
int compareType(Type *type1, Type *type2)
{
  if (type1->typeClass == type2->typeClass)
  {
    if (type1->typeClass == TP_ARRAY)
    {
      if (type1->arraySize == type2->arraySize)
        return compareType(type1->elementType, type2->elementType);
      else
        return 0;
    }
    else
      return 1;
  }
  else
    return 0;
}

/**
 * Hàm freeType: Giải phóng bộ nhớ của kiểu dữ liệu
 * @param type: Kiểu dữ liệu cần giải phóng
 *
 * Xử lý:
 * - INT, CHAR: chỉ free struct Type
 * - ARRAY: phải free đệ quy elementType trước, rồi mới free struct Type
 *   (tránh memory leak khi mảng lồng nhau)
 */
void freeType(Type *type)
{
  switch (type->typeClass)
  {
  case TP_INT:
  case TP_CHAR:
    free(type);
    break;
  case TP_ARRAY:
    freeType(type->elementType);
    freeType(type);
    break;
  }
}

/******************* Constant utility ******************************/

/**
 * Hàm makeIntConstant: Tạo giá trị hằng số kiểu Integer
 * @param i: Giá trị số nguyên
 * @return: Con trỏ đến ConstantValue chứa giá trị integer
 *
 * Ví dụ: CONST N = 10; -> makeIntConstant(10)
 */
ConstantValue *makeIntConstant(int i)
{
  ConstantValue *value = (ConstantValue *)malloc(sizeof(ConstantValue));
  value->type = TP_INT;
  value->intValue = i;
  return value;
}

/**
 * Hàm makeCharConstant: Tạo giá trị hằng số kiểu Character
 * @param ch: Ký tự
 * @return: Con trỏ đến ConstantValue chứa giá trị character
 *
 * Ví dụ: CONST C = 'A'; -> makeCharConstant('A')
 */
ConstantValue *makeCharConstant(char ch)
{
  ConstantValue *value = (ConstantValue *)malloc(sizeof(ConstantValue));
  value->type = TP_CHAR;
  value->charValue = ch;
  return value;
}

/**
 * Hàm duplicateConstantValue: Sao chép giá trị hằng số
 * @param v: Giá trị hằng cần sao chép
 * @return: Bản sao mới của giá trị hằng
 *
 * Copy giá trị phù hợp theo type:
 * - Nếu TP_INT: copy intValue
 * - Nếu TP_CHAR: copy charValue
 */
ConstantValue *duplicateConstantValue(ConstantValue *v)
{
  ConstantValue *value = (ConstantValue *)malloc(sizeof(ConstantValue));
  value->type = v->type;
  if (v->type == TP_INT)
    value->intValue = v->intValue;
  else
    value->charValue = v->charValue;
  return value;
}

/******************* Object utilities ******************************/

/**
 * Hàm createScope: Tạo scope (phạm vi) mới
 * @param owner: Object sở hữu scope này (Program, Function, Procedure)
 * @param outer: Scope bên ngoài (scope cha)
 * @return: Con trỏ đến Scope mới
 *
 * Cấu trúc scope lồng nhau:
 * Program Scope (outer = NULL)
 *   └─ Function Scope (outer = Program Scope)
 *       └─ Local Scope (outer = Function Scope)
 *
 * objList ban đầu = NULL, sẽ được thêm object khi khai báo
 */
Scope *createScope(Object *owner, Scope *outer)
{
  Scope *scope = (Scope *)malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

/**
 * Hàm createProgramObject: Tạo object Program (root của symbol table)
 * @param programName: Tên chương trình
 * @return: Con trỏ đến Program object
 *
 * Xử lý:
 * 1. Tạo object với kind = OBJ_PROGRAM
 * 2. Tạo scope cho program với outer = NULL (scope ngoài cùng)
 * 3. Gán vào symtab->program (gốc của cây symbol table)
 *
 * Ví dụ: PROGRAM Example2; -> createProgramObject("Example2")
 */
Object *createProgramObject(char *programName)
{
  Object *program = (Object *)malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes *)malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program, NULL);
  symtab->program = program;

  return program;
}

/**
 * Hàm createConstantObject: Tạo object Constant (hằng số)
 * @param name: Tên hằng số
 * @return: Con trỏ đến Constant object
 *
 * Chưa gán giá trị (value), sẽ được gán sau bởi compileConstant()
 * Ví dụ: CONST N = 10; -> createConstantObject("N")
 */
Object *createConstantObject(char *name)
{
  Object *obj = (Object *)malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_CONSTANT;
  obj->constAttrs = (ConstantAttributes *)malloc(sizeof(ConstantAttributes));
  return obj;
}

/**
 * Hàm createTypeObject: Tạo object Type (định nghĩa kiểu tùy chỉnh)
 * @param name: Tên kiểu
 * @return: Con trỏ đến Type object
 *
 * Cho phép người dùng định nghĩa kiểu mới
 * Ví dụ: TYPE IntArray = ARRAY [10] OF INTEGER;
 *        -> createTypeObject("IntArray")
 */
Object *createTypeObject(char *name)
{
  Object *obj = (Object *)malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_TYPE;
  obj->typeAttrs = (TypeAttributes *)malloc(sizeof(TypeAttributes));
  return obj;
}

/**
 * Hàm createVariableObject: Tạo object Variable (biến)
 * @param name: Tên biến
 * @return: Con trỏ đến Variable object
 *
 * Lưu scope hiện tại vào varAttrs->scope để biết biến thuộc scope nào
 * Ví dụ: VAR n : INTEGER; -> createVariableObject("n")
 */
Object *createVariableObject(char *name)
{
  Object *obj = (Object *)malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_VARIABLE;
  obj->varAttrs = (VariableAttributes *)malloc(sizeof(VariableAttributes));
  obj->varAttrs->scope = symtab->currentScope;
  return obj;
}

/**
 * Hàm createFunctionObject: Tạo object Function (hàm)
 * @param name: Tên hàm
 * @return: Con trỏ đến Function object
 *
 * Xử lý:
 * - Tạo scope mới cho function (outer = currentScope)
 * - paramList = NULL, sẽ được thêm khi compile params
 * - returnType sẽ được gán sau
 *
 * Ví dụ: FUNCTION F(n : INTEGER) : INTEGER;
 *        -> createFunctionObject("F")
 */
Object *createFunctionObject(char *name)
{
  Object *obj = (Object *)malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_FUNCTION;
  obj->funcAttrs = (FunctionAttributes *)malloc(sizeof(FunctionAttributes));
  obj->funcAttrs->paramList = NULL;
  obj->funcAttrs->scope = createScope(obj, symtab->currentScope);
  return obj;
}

/**
 * Hàm createProcedureObject: Tạo object Procedure (thủ tục)
 * @param name: Tên procedure
 * @return: Con trỏ đến Procedure object
 *
 * Tương tự Function nhưng không có returnType
 * Procedure chỉ thực thi công việc, không trả về giá trị
 *
 * Ví dụ: PROCEDURE WriteLn(); -> createProcedureObject("WriteLn")
 */
Object *createProcedureObject(char *name)
{
  Object *obj = (Object *)malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PROCEDURE;
  obj->procAttrs = (ProcedureAttributes *)malloc(sizeof(ProcedureAttributes));
  obj->procAttrs->paramList = NULL;
  obj->procAttrs->scope = createScope(obj, symtab->currentScope);
  return obj;
}

/**
 * Hàm createParameterObject: Tạo object Parameter (tham số)
 * @param name: Tên tham số
 * @param kind: Loại tham số (PARAM_VALUE hoặc PARAM_REFERENCE)
 * @param owner: Function/Procedure chứa tham số này
 * @return: Con trỏ đến Parameter object
 *
 * Hai loại tham số:
 * - PARAM_VALUE: truyền giá trị (n : INTEGER)
 * - PARAM_REFERENCE: truyền tham chiếu (VAR x : INTEGER)
 *
 * Lưu owner để biết parameter thuộc function/procedure nào
 */
Object *createParameterObject(char *name, enum ParamKind kind, Object *owner)
{
  Object *obj = (Object *)malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PARAMETER;
  obj->paramAttrs = (ParameterAttributes *)malloc(sizeof(ParameterAttributes));
  obj->paramAttrs->kind = kind;
  obj->paramAttrs->function = owner;
  return obj;
}

void freeObject(Object *obj)
{
  switch (obj->kind)
  {
  case OBJ_CONSTANT:
    free(obj->constAttrs->value);
    free(obj->constAttrs);
    break;
  case OBJ_TYPE:
    free(obj->typeAttrs->actualType);
    free(obj->typeAttrs);
    break;
  case OBJ_VARIABLE:
    free(obj->varAttrs->type);
    free(obj->varAttrs);
    break;
  case OBJ_FUNCTION:
    freeReferenceList(obj->funcAttrs->paramList);
    freeType(obj->funcAttrs->returnType);
    freeScope(obj->funcAttrs->scope);
    free(obj->funcAttrs);
    break;
  case OBJ_PROCEDURE:
    freeReferenceList(obj->procAttrs->paramList);
    freeScope(obj->procAttrs->scope);
    free(obj->procAttrs);
    break;
  case OBJ_PROGRAM:
    freeScope(obj->progAttrs->scope);
    free(obj->progAttrs);
    break;
  case OBJ_PARAMETER:
    freeType(obj->paramAttrs->type);
    free(obj->paramAttrs);
  }
  free(obj);
}

void freeScope(Scope *scope)
{
  freeObjectList(scope->objList);
  free(scope);
}

void freeObjectList(ObjectNode *objList)
{
  ObjectNode *list = objList;

  while (list != NULL)
  {
    ObjectNode *node = list;
    list = list->next;
    freeObject(node->object);
    free(node);
  }
}

void freeReferenceList(ObjectNode *objList)
{
  ObjectNode *list = objList;

  while (list != NULL)
  {
    ObjectNode *node = list;
    list = list->next;
    free(node);
  }
}

/**
 * Hàm addObject: Thêm object vào cuối danh sách liên kết
 * @param objList: Con trỏ đến con trỏ danh sách object
 * @param obj: Object cần thêm
 *
 * Xử lý:
 * 1. Tạo node mới chứa object
 * 2. Nếu danh sách rỗng: gán node làm đầu danh sách
 * 3. Nếu không: duyệt đến cuối danh sách và thêm vào
 *
 * Danh sách liên kết đơn: obj1 -> obj2 -> obj3 -> NULL
 */
void addObject(ObjectNode **objList, Object *obj)
{
  ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL)
    *objList = node;
  else
  {
    ObjectNode *n = *objList;
    while (n->next != NULL)
      n = n->next;
    n->next = node;
  }
}

/**
 * Hàm findObject: Tìm object theo tên trong một danh sách
 * @param objList: Danh sách object cần tìm
 * @param name: Tên object cần tìm
 * @return: Object nếu tìm thấy, NULL nếu không tìm thấy
 *
 * Duyệt tuần tự từ đầu đến cuối danh sách
 * So sánh tên bằng strcmp (phân biệt hoa thường)
 */
Object *findObject(ObjectNode *objList, char *name)
{
  while (objList != NULL)
  {
    if (strcmp(objList->object->name, name) == 0)
      return objList->object;
    else
      objList = objList->next;
  }
  return NULL;
}

/******************* others ******************************/

/**
 * Hàm initSymTab: Khởi tạo Symbol Table với các hàm/thủ tục built-in
 *
 * Tạo Symbol Table và thêm các hàm/thủ tục có sẵn của ngôn ngữ:
 * - READC(): đọc một ký tự, trả về CHAR
 * - READI(): đọc một số nguyên, trả về INTEGER
 * - WRITEI(i: INTEGER): in số nguyên
 * - WRITEC(ch: CHAR): in ký tự
 * - WRITELN(): xuống dòng
 *
 * Cũng tạo các kiểu dữ liệu cơ bản: intType, charType
 */
void initSymTab(void)
{
  Object *obj;
  Object *param;

  symtab = (SymTab *)malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;

  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList), param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList), param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

/**
 * Hàm cleanSymTab: Giải phóng toàn bộ bộ nhớ của Symbol Table
 *
 * Giải phóng theo thứ tự:
 * 1. Program object (bao gồm cả scope và các object con)
 * 2. Global object list (các hàm built-in)
 * 3. Symbol table chính
 * 4. Các kiểu dữ liệu cơ bản
 */
void cleanSymTab(void)
{
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

/**
 * Hàm enterBlock: Vào một scope (block) mới
 * @param scope: Scope cần vào
 *
 * Chuyển currentScope sang scope mới (thường là scope con)
 * Dùng khi bắt đầu compile block của Program/Function/Procedure
 */
void enterBlock(Scope *scope)
{
  symtab->currentScope = scope;
}

/**
 * Hàm exitBlock: Thoát khỏi scope hiện tại, quay về scope ngoài
 *
 * Chuyển currentScope về scope->outer (scope cha)
 * Dùng khi kết thúc compile block của Program/Function/Procedure
 */
void exitBlock(void)
{
  symtab->currentScope = symtab->currentScope->outer;
}

/**
 * Hàm lookupObject: Tìm kiếm object theo tên trong symbol table
 * @param name: Tên object cần tìm
 * @return: Object nếu tìm thấy, NULL nếu không
 *
 * Quy tắc tìm kiếm (Lexical Scoping):
 * 1. Tìm trong scope hiện tại
 * 2. Nếu không thấy, tìm trong scope ngoài (outer)
 * 3. Lặp lại cho đến scope ngoài cùng (Program scope)
 * 4. Nếu vẫn không thấy, tìm trong globalObjectList (built-in functions)
 *
 * Ví dụ scope lồng nhau:
 *   Program { VAR x }  <- scope 3
 *     Function F { VAR y }  <- scope 2
 *       BEGIN { VAR z }  <- scope 1 (current)
 *
 * Tìm z: tìm thấy ở scope 1
 * Tìm y: không có ở scope 1, tìm ở scope 2, tìm thấy
 * Tìm x: không có ở scope 1, 2, tìm ở scope 3, tìm thấy
 * Tìm WRITEI: không có ở scope 1,2,3, tìm ở globalObjectList, tìm thấy
 */
Object *lookupObject(char *name)
{
  Scope *scope = symtab->currentScope;
  Object *obj = NULL;

  // Duyệt qua các scope từ trong ra ngoài
  while (scope != NULL)
  {
    // Tìm trong danh sách object của scope hiện tại
    obj = findObject(scope->objList, name);
    if (obj != NULL)
      return obj;

    // Chuyển sang scope bên ngoài
    scope = scope->outer;
  }

  // Nếu không tìm thấy trong các scope, tìm trong global object list
  obj = findObject(symtab->globalObjectList, name);

  return obj;
}

/**
 * Hàm declareObject: Khai báo object vào symbol table
 * @param obj: Object cần khai báo
 *
 * Xử lý đặc biệt cho Parameter:
 * - Thêm vào paramList của Function/Procedure (để lưu thứ tự tham số)
 * - Đồng thời thêm vào objList của scope (để lookup được)
 *
 * Các object khác:
 * - Chỉ thêm vào objList của scope hiện tại
 *
 * Ví dụ:
 *   FUNCTION F(n : INTEGER)  <- n được thêm vào:
 *     1. F->funcAttrs->paramList (danh sách tham số)
 *     2. F->funcAttrs->scope->objList (để lookup trong function)
 */
void declareObject(Object *obj)
{
  if (obj->kind == OBJ_PARAMETER)
  {
    Object *owner = symtab->currentScope->owner;
    switch (owner->kind)
    {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }

  addObject(&(symtab->currentScope->objList), obj);
}
