#ifndef TPF
    #define TPF 100  // ticks per frame (like FPS in games)
#endif
#ifndef SNAKE_LEN
    #define SNAKE_LEN 50
#endif
#ifndef SNAKE_COLOR
    #define SNAKE_COLOR 200
#endif
#ifndef BG_COLOR
    #define BG_COLOR 0
#endif
#ifndef POINT_SIZE
    #define POINT_SIZE 3
#endif

#ifndef SNAKE_X_START
    #define SNAKE_X_START 0
#endif
#ifndef SNAKE_X_END
    #define SNAKE_X_END 230
#endif
#ifndef SNAKE_Y_START
    #define SNAKE_Y_START 100
#endif
#ifndef SNAKE_Y_END
    #define SNAKE_Y_END 310
#endif



typedef enum Direction {
  TOP,
  RIGHT,
  BOTTOM,
  LEFT
} Direction;


typedef struct Point {
    int x;
    int y;
} Point;


typedef struct SnakePointT {
    Point position;
    struct SnakePointT * next;
    struct SnakePointT * prev;
} SnakePoint;


void drow_point(Point point, int color);

void SNAKE_push_point(Point point, SnakePoint ** head, SnakePoint ** tail);

void SNAKE_remove_last_point(SnakePoint ** tail);

void SNAKE_init(SnakePoint ** head, SnakePoint ** tail, int len, int point_size);

Point SNAKE_add_point(SnakePoint ** head, SnakePoint ** tail, Direction direction, int point_size);
