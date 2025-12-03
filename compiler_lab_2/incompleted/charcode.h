/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __CHARCODE_H__
#define __CHARCODE_H__

// Enum liệt kê các loại ký tự (character types)
// Dùng để phân loại ký tự khi phân tích từ vựng (lexer)
typedef enum
{
  CHAR_SPACE,        // Ký tự khoảng trắng: ' ', '\t', '\n'
  CHAR_LETTER,       // Chữ cái: A-Z, a-z
  CHAR_DIGIT,        // Chữ số: 0-9
  CHAR_PLUS,         // Dấu cộng: '+'
  CHAR_MINUS,        // Dấu trừ: '-'
  CHAR_TIMES,        // Dấu nhân: '*'
  CHAR_SLASH,        // Dấu chia: '/'
  CHAR_LT,           // Nhỏ hơn: '<'
  CHAR_GT,           // Lớn hơn: '>'
  CHAR_EXCLAIMATION, // Dấu chấm than: '!'
  CHAR_EQ,           // Dấu bằng: '='
  CHAR_COMMA,        // Dấu phẩy: ','
  CHAR_PERIOD,       // Dấu chấm: '.'
  CHAR_COLON,        // Dấu hai chấm: ':'
  CHAR_SEMICOLON,    // Dấu chấm phẩy: ';'
  CHAR_SINGLEQUOTE,  // Dấu nháy đơn: '''
  CHAR_LPAR,         // Dấu ngoặc trái: '('
  CHAR_RPAR,         // Dấu ngoặc phải: ')'
  CHAR_UNKNOWN       // Ký tự không xác định / không thuộc nhóm nào
} CharCode;

#endif
