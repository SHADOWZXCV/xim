#ifndef XIM_TYPES_H_
#define XIM_TYPES_H_

typedef struct {
    int x;
    int y;
} Vector2d;

typedef struct {
    unsigned short width;
    unsigned short height;
} Size2s;

typedef struct {
  unsigned short keyCode;
  unsigned short character;  
} KeyCode;

enum DATA_TYPES {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_STRING,
  TYPE_SHORT,
  TYPE_INT8,
  TYPE_INT16,
  TYPE_INT32,
  TYPE_INT64,
  TYPE_UINT8,
  TYPE_UINT16,
  TYPE_UINT32,
  TYPE_UINT64,
  TYPE_UNSIGNED,
  TYPE_UNSIGNED_SHORT,
  TYPE_UNSIGNED_INT,
  TYPE_UNSIGNED_LONG,
  TYPE_LONG,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_BOOL,
  TYPE_VOID,
  TYPE_POINTER,
  TYPE_SIZE_T,
  TYPE_SSIZE_T,
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_ENUM,
  TYPE_FUNCTION,
  TYPE_UNKNOWN
};

#endif