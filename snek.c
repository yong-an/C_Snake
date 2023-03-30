#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define width 100       // console window width
#define height 30       // console window height
#define snekLength 4    // initial length of snek
#define life 3          // starting lives
#define speed 88        // snek speed [ higher = slower ]

#define U 1             // using a macro to replace the action direction of a snake 
#define D 2             // each direction is represented by a number
#define L 3             // Up Down Left Right : 1 2 3 4
#define R 4   

typedef struct snake
{
   int x;
   int y;
   struct snake* next;
}snake;

int foodx, foody;       // for generating food x y coordinates 
snake *head;            // pointer to point to snake head LL
snake *tail;            // pointer to point to snake tail LL
snake *p = NULL;        // temp pointer for manipulating LL
snake *nexthead;        // pointer to point to the next head
int status = R;         // snek default status to be facing right
int score = 0;          // score counter
int tries = life - 1;   // just a variable to store tries left, must - 1 because ends at 0

// cursor must be hidden in the cmd if not there are alot of UI glitches
void hideCursor()
{
   CONSOLE_CURSOR_INFO cursor_info = {1, 0};
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// function to set cursor at a specific X Y coordinates
void goToXY(int x, int y)
{
   COORD pos = {x, y};
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// function to generate the map zone
void generateMap() 
{
   int i;
   // generating the up down borders
   for(i = 0; i < width - 3; i++)
   {
      goToXY(i, 26);
      printf("===");
      goToXY(i, 0);
      printf("===");
   }
   // generating the left right borders
   for (i = 0; i < height - 3; i++) 
   {
      goToXY(0, i);
      printf("I");
      goToXY(99, i);
      printf("I");
   }
   // generating the score board
   goToXY(24, 28);
   printf ("Score: 0 , [Space] to Pause , Life:%d", life); 
}

// function to generate the snek food
void generateFood()
{
   // seed to randomize using current time
   srand(time(NULL)); 

   // maximun value of random number to not go out of the map
   foodx = rand() % (width - 4) + 2; 
   foody = rand() % (height - 8) + 2;

   // if the food x coordinates generated is not even number, regenerate again. *
   while (foodx % 2) 
      foodx = rand() % (width - 4) + 2; 
   
   // bring cursor to that loation and print the food
   goToXY(foodx, foody);
   printf("*");
}

// initialize the location of the snek
void generateSnek()
{ 
    // generate 1 LL node first
    tail = (snake*)malloc(sizeof(snake));
    tail -> next = NULL;
    tail -> x = height - 6;
    tail -> y = 8;

    // for loop here to generate the rest of the tail 
    for (int i = 1; i <= snekLength; i++) 
    {
       head = (snake*)malloc(sizeof(snake));
       head -> next = tail;

       // head -> x must be an even number, this is to correspond to the food x coordinates
       head -> x = 24 + i * 2;
       head -> y = 8;

       // head of another LL node is also the tail of the next LL node
       tail = head;  
    }

    // shift to the front of the LL to print the head which is " O "
    while (tail)
    {
       goToXY(tail -> x, tail -> y);
       printf("O");
       tail = tail -> next;
    }
}

// function to translate keystrokes into status which is represented in integer and hence controlling the snek
void checkKeyStrokes() 
{
   char ch = _getch();
   switch (ch)
   {
      // prevents the game from crashing by not allowing double input of opposite directions
      case 'w': 
      if(status != D)
         status = U; break;
      case 's': 
      if (status != U)
         status = D; break;
      case 'a': 
      if (status != R)
         status = L; break;
      case 'd': 
      if (status != L)
         status = R; break;
      // space bar to pause the game
      case ' ': 
         _getch(); break; 
   }
}

// function that will be called, when you bite into your own tail
int congratulationsYouPlayedYourself() 
{
    // self is a node on the snek
    snake *self = head -> next; 

    // comparing the members of head and self, since snake head always exists, and the snake body (tail) is traversed here
    // which is to say if the head of my LL matches the X Y coordinates of any of my body (tail) it means congratulationsYouPlayedYourself
    while (self) {
       if (self -> x == head -> x && self -> y == head -> y)
         return 1;
       
       self = self -> next;
    }

    return 0;
}

// function that will be called, if you langar the wall
int giveTheSnekMinusOne()
{
   if (head -> x <= 1 || head -> x >= 98 || head -> y <= 0 || head -> y >= 26)
      return 1;

   return 0;
}

// function to display the game over screen and reseting the screen
void gameOver() 
{
   // point to the head of the snek
   p = head;  

   // free the memory 
   while (head)
   {
      head = head -> next;
      free(p);
      p = head;
   }

   // clear cmd
   system("cls");
   goToXY(45, 12);

   // display message
   printf ("Game Over!");
   goToXY(44, 14);

   // display score
   printf ("Final Score:%d", score);
   goToXY(0,28);
}

// function to refresh the game state and update the life information
void refresh() 
{
   // use the temp pointer to point to the location where the snek dieded, save the X Y coordinates first
   p = head;
   int _x_ = p -> x; 
   int _y_ = p -> y;

   // while loop to clear the entire snek dead body 
   while (head) {
      goToXY(head -> x, head -> y);
      printf(" ");
      head = head -> next;
      free(p);
      p = head;
   }

   // update the player's life count
   goToXY(52, 28);
   printf(", Quick Mefs -> Life Left: %d", tries);
}

// main logic of the snek is here
void snek() 
{  
   nexthead = (snake*)malloc(sizeof(snake));

   // determine the next X Y coordinates of the new snek head on the game board
   // 4 directions
   if (status == U)
   {
      nexthead -> y = head -> y - 1;
      nexthead -> x = head -> x;
   }
   if (status == D)
   {
      nexthead -> y = head -> y + 1;
      nexthead -> x = head -> x;
   }
   if (status == L)
   {
      nexthead -> x = head -> x - 2;
      nexthead -> y = head -> y;
   }
   if (status == R)
   {
      nexthead -> x = head -> x + 2;
      nexthead -> y = head -> y;
   }

   // operations to add the new head on top of the LL
   nexthead -> next = head;
   head = nexthead;
   p = head;

   // determine whether the position of the snake head coincides with the position of the food [ aka the snek ate 1 pax of food ]
   if (p -> x == foodx && p -> y == foody) 
   {
      // output the tail node
      // because each motion creates a new head node and then deletes a tail node.
      // therefore, to add a body, you only need to output the tail node once without releasing the tail node in this cycle
      while (p)
      {
         goToXY(p -> x, p -> y);
         if (p == head)
            printf("O");
         else
            printf("\x04");
         p = p -> next;  
      }
       
       // after snek makan finish generate another food
       generateFood(); 

       // update scores
       score++;
       goToXY(32, 28);
       printf("%d", score);
    }
    else
    {
      // do not output the tail node
      while (p -> next -> next) 
      {
         goToXY(p -> x, p -> y);
         if (p == head)
            printf("O");
         else
            printf("\x04");
         p = p -> next;
      }

       // remove the tail node (if this is not here it will leave a snek trail)
       goToXY(p -> next -> x, p -> next -> y);
       printf(" ");
       free(p -> next);
       p -> next = NULL;
    }

    p = head;

   // plausible scenario, in which when the food is spawn on the same location as the snek
   while (p)
   {
      if (p -> x == foodx && p -> y == foody)
         generateFood();
      p = p -> next;
   }
}

// main function
int main() 
{
   system("mode con cols=100 lines=30");                    // Set console size
   system ("title Snek Game By yong-an");                    // Set title
   hideCursor();                                            // hide cursor
   generateMap();                                           // initialize map and information
   generateFood();                                          // initialize to randomly generate a food

   for (; tries >= 0; tries--)                              // as long as there are tries remaining
   {                                                        
      generateSnek();                                       // initialize the location of the snek
      status = R;                                           // initialize starting direction of motion
 
      while (1)           
      {
         snek();                                            // snek action animations, the direction is controlled by the control variable

         if (_kbhit())                                      // if receive keystrokes in cmd
            checkKeyStrokes();                              // receive keyboard keys and translate into movement
         
         if (congratulationsYouPlayedYourself() || giveTheSnekMinusOne())     // two conditions for dropping life
            break;

         Sleep(speed);                                      // set the snek movement speed    
      }

      refresh();                                            // clear the traces left by the last snek and update the life information
   }

   gameOver();                                              // game over ending interface

   return 0;
}

//Compile using gcc -o [any name] snek.c

