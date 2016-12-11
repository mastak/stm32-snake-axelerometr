#define TPF 100  // ticks per frame (like FPS in games)
#define SNAKE_LEN 50
#define SNAKE_COLOR 240
#define BG_COLOR 0
#define POINT_SIZE 3
#define SNAKE_X_START 0
#define SNAKE_X_END 230
#define SNAKE_Y_START 100
#define SNAKE_Y_END 310


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
