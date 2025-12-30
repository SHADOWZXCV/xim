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

#endif