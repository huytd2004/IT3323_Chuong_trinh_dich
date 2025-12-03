/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __CHARCODE_H__
#define __CHARCODE_H__

// CharCode dùng để phân loại ký tự trong scanner
typedef enum
{
  CHAR_SPACE,        // Ký tự trắng: space, tab, newline
  CHAR_LETTER,       // Chữ cái A-Z, a-z
  CHAR_DIGIT,        // Chữ số 0-9
  CHAR_PLUS,         // Dấu cộng '+'
  CHAR_MINUS,        // Dấu trừ '-'
  CHAR_TIMES,        // Dấu nhân '*'
  CHAR_SLASH,        // Dấu chia '/'
  CHAR_LT,           // Dấu bé hơn '<'
  CHAR_GT,           // Dấu lớn hơn '>'
  CHAR_EXCLAIMATION, // Dấu chấm than '!'
  CHAR_EQ,           // Dấu bằng '='
  CHAR_COMMA,        // Dấu phẩy ','
  CHAR_PERIOD,       // Dấu chấm '.'
  CHAR_COLON,        // Dấu hai chấm ':'
  CHAR_SEMICOLON,    // Dấu chấm phẩy ';'
  CHAR_SINGLEQUOTE,  // Dấu nháy đơn '''
  CHAR_LPAR,         // Dấu ngoặc trái '('
  CHAR_RPAR,         // Dấu ngoặc phải ')'
  CHAR_UNKNOWN       // Ký tự không xác định hoặc không hợp lệ
} CharCode;

#endif
