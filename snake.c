#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

// 'arrow' key constants (W, S, A, D ASCII values)
#define UP_KEY 119
#define DOWN_KEY 115
#define LEFT_KEY 97
#define RIGHT_KEY 100

// direction constants
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

// game constants
#define WIDTH 40
#define HEIGHT 14
#define TICK_RATE 0.1

// hides the cursor (taken from Stack Overflow)
void hidecursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// goes to a certain position in the console
void gotoxy(int x,int y)
{
	printf("%c[%d;%df",0x1B,y,x);
}

// represents a snake segment
typedef struct segment {
    int x;
    int y;
    struct segment* prev;
    struct segment* next;
} segment_t;

// represents the snake
typedef struct snake {
    segment_t* head;
    segment_t* tail;
    int size;
    int dir;
} snake_t;

// print the game board
void print_game(snake_t* snake)
{
    // create a pointer to the head of the snake
    segment_t* seg_point = snake->head;
    // loop over snake and print out segment characters
    for (int i = 0; i < snake->size; i++) 
    {
        gotoxy(seg_point->x, seg_point->y);
        printf("%c", '0');
        seg_point = seg_point->next;
    }
}

// change the position of snake head by x and y
void move_head(snake_t* snake)
{
    // declare variables
    int x = snake->head->x;
    int y = snake->head->y;
    // check the direction
    switch(snake->dir)
    {
        case UP:
            y += -1;
            break;
        case DOWN:
            y += 1;
            break;
        case LEFT:
            x += -1;
            break;
        case RIGHT:
            x += 1;
            break;
        default:
            break;
    }
    // set values
    snake->head->x = x;
    snake->head->y = y;
}

// print the current food piece on the screen
void print_food(int food_x, int food_y)
{
    gotoxy(food_x, food_y);
    // set color to green
    printf("\033[0;32m");
    printf("%c", 'X');
    // set color back to white
    printf("\033[0;37m");
}

// add a segment to the snake
void add_segment(snake_t* snake, int x, int y)
{
    // create a new segment
    segment_t * new_seg = malloc(sizeof(segment_t));
    // set x and y
    new_seg->x = x;
    new_seg->y = y;
    // set next and prev
    new_seg->next = NULL;
    new_seg->prev = snake->tail;
    // change tail of snake next
    snake->tail->next = new_seg;
    // change snake tail pointer
    snake->tail = new_seg;
    // update snake size
    snake->size += 1;
}

// move all the snake segments forward one
void move_snake(snake_t* snake, int x_head, int y_head)
{
    if (snake->size > 1) {
        // create a pointer to the snake's tail
        segment_t* tail_point = snake->tail;

        // loop over segments in reverse
        for (int i=snake->size - 2; i > 0; i-=1)
        {
            // set x and y to the previous x and y
            tail_point->x = tail_point->prev->x;
            tail_point->y = tail_point->prev->y;
            // move the pointer
            tail_point = tail_point->prev;
        }
        // move the last one next to head
        tail_point->x = x_head;
        tail_point->y = y_head;
    }
}

// randomly generate food
void gen_food(snake_t* snake, int *food_x, int *food_y)
{
    
    int valid_posn = 0;
    while (!valid_posn) {
        // generate new numbers
        *food_x = rand() % (WIDTH - 2) + 2;
        *food_y = rand() % (HEIGHT - 2) + 2;
        // see if position is valid (not on the snake)
        valid_posn = 1;
        segment_t* seg_point = snake->head;
        for (int i = 0; i < snake->size; i++) {
            if (*food_x == seg_point->x || *food_y == seg_point->y) {
                valid_posn = 0;
            }
            seg_point = seg_point->next;
        }
    }
    // print the food on the screen
    print_food(*food_x, *food_y);
}

// has the snake eaten a piece of food?
void eat_food(snake_t* snake, int *food_x, int *food_y)
{
    // get snake head positions
    int head_x = snake->head->x;
    int head_y = snake->head->y;
    // is the snake's head on the piece of food?
    if (
        (head_x == *food_x) &&
        (head_y == *food_y)
    ){
        // need to handle moving the snake forward and eating food
        int new_x = snake->tail->x;
        int new_y = snake->tail->y;
        // move and print?

        // add new segment and print it?
        add_segment(snake, new_x, new_y);
        gotoxy(new_x, new_y);
        printf("%c", '0');

        // gen new food
        gen_food(snake, food_x, food_y);
    }
}

int collide_with_itself(snake_t* snake)
{
    // check if size allows for collision
    if (snake->size > 3) {
        // create pointer to point to each segment
        segment_t* seg_point = snake->head->next;
        // loop over all segments and compare
        for (int i = 1; i < snake->size; i++)
        {
            // compare the x and y of head and current segment
            if (
                (snake->head->x == seg_point->x) &&
                (snake->head->y == seg_point->y)
            ) {
                return 1;
            }
            seg_point = seg_point->next;
        }
    }
    // snake did not collide with itself
    return 0;
}

// check if the snake collided with the wall
int collide_with_wall(snake_t* snake)
{
    // check if it is out of bounds
    if (
        (snake->head->x <= 1 || snake->head->x >= WIDTH) ||
        (snake->head->y <= 1 || snake->head->y >= HEIGHT)
    ){
        return 1;
    }
    return 0;
}

// move the snake forward and re-print the head and tail
void move_and_print(snake_t* snake)
{
    // store old head x and y
    int head_x = snake->head->x;
    int head_y = snake->head->y;

    int tail_x = snake->tail->x;
    int tail_y = snake->tail->y;

    // move the head
    move_head(snake);
    // print the new head   
    gotoxy(snake->head->x, snake->head->y);
    printf("%c", '0');

    move_snake(snake, head_x, head_y);
 
    // clear the tail
    gotoxy(tail_x, tail_y);
    printf("%c", (char) ' ');
}

// free all the snake memory
void free_snake(snake_t* snake)
{
    // make a pointer to the tail to move
    segment_t* seg_point = snake->tail;
    segment_t* temp_point = snake->tail;

    // loop over all segments
    while (seg_point->prev != NULL)
    {
        // clear the print
        gotoxy(seg_point->x, seg_point->y);
        printf("%c", ' ');
        // free the actual memory
        seg_point = seg_point->prev;
        free(temp_point);
        temp_point = seg_point;
    }
}

// re-print the blank spaces
void print_board()
{
    // print all the space
    for (int i = 1; i < WIDTH - 1; i++) 
    {
        for (int j = 1; j < HEIGHT - 1; j++)
        {
            gotoxy(i, j);
            printf("%c", ' ');
        }
    }

    // print the sides
    for (int j = 1; j <= HEIGHT; j++)
    {
        gotoxy(0, j);
        printf("%c", 'x');
        gotoxy(WIDTH, j);
        printf("%c", 'x');
    }

    
    // print the top/bottom
    for (int i = 1; i <= WIDTH; i++)
    {
        gotoxy(i, 0);
        printf("%c", 'x');
        gotoxy(i, HEIGHT);
        printf("%c", 'x');
    }
}

// print the game over message
void print_game_over(int score)
{
    // go to middle of screen (minus half of 'game over' length)
    gotoxy((WIDTH/2) - 4, (HEIGHT/2) - 1);
    // print in red
    printf("\33[1;31m");
    printf("GAME OVER");
    printf("\33[37m");
    // print score
    gotoxy((WIDTH/2) - 4, (HEIGHT/2));
    printf("score: %i", score);
    // print options
    gotoxy((WIDTH/2) - 9, (HEIGHT/2) + 1);
    printf("press '1' to replay");
    gotoxy((WIDTH/2) - 8, (HEIGHT/2) + 2);
    printf("press '5' to quit");
}


// clear the food
void clear_food(int food_x, int food_y)
{
    gotoxy(food_x, food_y);
    printf("%c", ' ');
}

// clear the head
void clear_head(int head_x, int head_y)
{
    gotoxy(head_x, head_y);
    printf("%c", ' ');
}

// handle a key press during gameplay
void handle_keypress_gameplay(int key, snake_t* snake, int *food_x, int *food_y) 
{
    // handle all options
    switch(key)
    {
        // move the snake up
        case UP_KEY:
            // check if going down
            if (!(snake->dir == DOWN)) {
                // change direction
                snake->dir = UP;
            }
            break;

        // move the snake down
        case DOWN_KEY:
            // check if going up
            if (!(snake->dir == UP)) {
                // change direction
                snake->dir = DOWN;
            }
            break;

        // move the snake left
        case LEFT_KEY:
            // check if going right
            if (!(snake->dir == RIGHT)) {
                // change direction
                snake->dir = LEFT;
            }
            break;

        // move the snake right
        case RIGHT_KEY:
            // check if going left
            if (!(snake->dir == LEFT)) {
                // change direction
                snake->dir = RIGHT;
            }
            break;
            
        default:
            break;
    }
}

// main gameplay
void gameplay_loop()
{
    // initialize snake
    snake_t game_snake;
    segment_t first_segment;

    first_segment.x = 3;
    first_segment.y = 3;
    first_segment.next = NULL;
    first_segment.prev = NULL;

    game_snake.head = &first_segment;
    game_snake.tail = &first_segment;
    game_snake.dir = RIGHT;
    game_snake.size = 1;

    // locations of the current food piece
    int food_x = 10;
    int food_y = 8;

    // initialize choice
    int choice = 0;

    // initialize time
    DWORD last_tick = GetTickCount();
    // initialize curr time
    DWORD curr_time;

    // print a bunch of empty space
    print_board();

    // print the snake
    print_game(&game_snake);

    // print the food
    print_food(food_x, food_y);

    int tick_count = 1;

    // store whether game is over or not
    int game_over = 0;

    // start loop
    do 
    {
        // tick rate
        curr_time = GetTickCount();
        // has the tick rate elapsed yet?
        if ((curr_time - last_tick) >= (TICK_RATE * 1000))
        {
            move_and_print(&game_snake);
            last_tick = curr_time;

            // check if the snake collided with itself or the wall
            if (collide_with_itself(&game_snake) ||
            (collide_with_wall(&game_snake))) 
            {
                // redraw the border
                gotoxy(game_snake.head->x, game_snake.head->y);
                printf("%c", 'x');
                // quit this loop
                game_over = 1;
                break;
            }

            // check if snake is the size of the board
            if (game_snake.size >= ((WIDTH-1)*(HEIGHT-1)))
            {
                game_over = 1;
                break; 
            }
            // check if snake has eaten food
            eat_food(&game_snake, &food_x, &food_y);
        }
        // get the choice
        if (kbhit() && game_over == 0)
        {
            choice = getch();
            // logic
            switch(choice)
            {
                case 53:
                    clear_head(game_snake.head->x, game_snake.head->y);
                    break;
                default:
                    handle_keypress_gameplay(choice, &game_snake,&food_x, &food_y);
                    break;
            }
        }

    } while (choice != 53);
    // end of game stuff
    free_snake(&game_snake);
    clear_food(food_x, food_y);
    print_game_over(game_snake.size);
}

// prints the main menu
void print_menu()
{
    printf("Press '1' to play and '5' to quit.");
    // more to come later
}

// control main flow of program
int main()
{
    int choice = 0;

    // hide the cursor
    hidecursor();
    // clear the screen
    system("cls");
    // print the menu
    print_menu();
    do
    {
        // hold until input
        choice = getch();
        // switch case to control screens
        switch(choice)
        {
            // start menu (option '0')
            case 48:
                break;
            // gameplay (option '1')
            case 49:
                gameplay_loop();
                break;
            // quit (option '5')
            case 53:
                system("cls");
                break;
            // default
            default:
                break;
        }
    } while(choice != 53); // run until user hits exit

    return 0;
}