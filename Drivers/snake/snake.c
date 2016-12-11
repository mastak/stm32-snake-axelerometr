#include <stdlib.h> 
#include "snake.h"


void drow_point(Point point, int color) {
  int i, k;
  for (i=0; i < POINT_SIZE; i++) {
    for (k=0; k < POINT_SIZE; k++) {
      LCD_DrawPixel(point.x + i, point.y + k, color);
    }
  }
}


void SNAKE_push_point(Point point, SnakePoint ** head, SnakePoint ** tail) {
    SnakePoint *new_point = (SnakePoint*) malloc(sizeof(SnakePoint));
    new_point->position.x = point.x;
    new_point->position.y = point.y;
    new_point->next = NULL;
    new_point->prev = NULL;

    if (*tail == NULL) {
        *tail = new_point;
    }

    if (head != NULL) {
        new_point->prev = *head;
        (*head)->next = new_point;
        *head = new_point;
    }
    *head = new_point;
}


void SNAKE_remove_last_point(SnakePoint ** tail) {
    SnakePoint * before_last = NULL;

    if (*tail == NULL) {
        return;
    }

    before_last = (*tail)->next;
    if (before_last != NULL) {
        before_last->prev = NULL;
    }
    free(*tail);
    *tail = before_last;
}


int SNAKE_point_is_exist(Point point, SnakePoint * head) {
    SnakePoint * work_head = NULL;
    while(work_head != NULL) {
        if (work_head->position.x == point.x && work_head->position.y == point.y) {
            return 1;    
        }
    }
    return 0;
}


void SNAKE_init(SnakePoint ** head, SnakePoint ** tail, int len, int point_size) {
    Point point;

    if (len > (SNAKE_X_END - SNAKE_X_START)) {
        return;
    }

    point.y = SNAKE_Y_START;
    point.x = SNAKE_X_END / 2;
    for (; (point.y - SNAKE_Y_START) < len * point_size; point.y += point_size) {
        SNAKE_push_point(point, head, tail);
    }
}

Point SNAKE_add_point(SnakePoint ** head, SnakePoint ** tail, Direction direction, int point_size) {
    Point new_point;
    int current_x = (*head)->position.x,
        current_y = (*head)->position.y;

    new_point.x = current_x;
    new_point.y = current_y;
    switch(direction) {
        case TOP:
            new_point.y = current_y > SNAKE_Y_START ? current_y - point_size : SNAKE_Y_END - point_size;
            break;
        case RIGHT:
            new_point.x = current_x < (SNAKE_X_END - point_size) ? current_x + point_size : SNAKE_X_START;
            break;
        case BOTTOM:
            new_point.y = current_y < (SNAKE_Y_END - point_size) ? current_y + point_size : SNAKE_Y_START;
            break;
        case LEFT:
            new_point.x = current_x > SNAKE_X_START ? current_x - point_size : SNAKE_X_END - point_size;
            break;
    }
    SNAKE_push_point(new_point, head, tail);    
}
