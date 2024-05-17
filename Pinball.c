#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define BLANK "   "

char symbol;
int racket_col, racket_top_row, racket_bot_row;
int ball_start_col, ball_start_row;
int top_row, bot_row, l_edge, r_edge;
float col_dir, row_dir;
int ndelay, delay;
time_t start, end;

int set_ticker(int n_msecs){
   struct itimerval new_timeset;
   long n_sec, n_usecs;
   
   n_sec = n_msecs / 1000;
   n_usecs = (n_msecs & 1000) * 1000L;
   
   new_timeset.it_interval.tv_sec = n_sec;
   new_timeset.it_interval.tv_usec = n_usecs;
   new_timeset.it_value.tv_sec = n_sec;
   new_timeset.it_value.tv_usec = n_usecs;
   
   return setitimer(ITIMER_REAL, &new_timeset, NULL); 
}

void move_msg(int signum){
   signal(SIGALRM, move_msg);
   move(ball_start_row, ball_start_col);
   addstr(BLANK);
   ball_start_row += row_dir;
   ball_start_col += col_dir; 
   move(ball_start_row, ball_start_col);
   addch(symbol);
   refresh();
   
   if(ball_start_col + col_dir <= l_edge){
      endwin();
      end = time(NULL);
      printf("Ping Pong Ball Game - Play Time: %ld(s).\n", end-start);
      exit(1);
   }
   if(ball_start_col + col_dir >= r_edge){
      ndelay = delay / 2;
      col_dir *= -1;
   }
   if(ball_start_row + row_dir >= bot_row || ball_start_row + row_dir <= top_row){
      ndelay = delay * 2;
      row_dir *= -1;
   }
   if(ball_start_col + col_dir == racket_col){
      if(ball_start_row >= racket_top_row && ball_start_row <= racket_bot_row)
      col_dir *= -1;
   }
   if(ndelay > 0){
         set_ticker(delay=ndelay);
         ndelay=0;
   }

   
}

void set_map(){
   int cur_edge = l_edge;
   int cur_row = top_row;
   move(top_row, l_edge);
   while(cur_edge <= r_edge){
      addch('*');
      cur_edge++;
      move(cur_row, cur_edge);
   }
   cur_edge--;
   move(cur_row, cur_edge);
   while(cur_row <= bot_row){
      addch('*');
      cur_row++;
      move(cur_row, cur_edge);
   }
   cur_row--;
   move(cur_row, cur_edge);
   while(cur_edge >= l_edge){
      addch('*');
      cur_edge--;
      move(cur_row, cur_edge);
   }
   cur_edge++;
   move(cur_row, cur_edge);
   while(cur_row > top_row){
      cur_row--;
      move(cur_row, cur_edge);
      if(cur_row > top_row)
      addch('>');
      
   }
   refresh();
}
void set_racket(){
   move(racket_top_row, racket_col);
   int i = racket_top_row;
   while(i <= racket_bot_row){
      addch('|');
      i++;
      move(i, racket_col);
   }
}

void move_racket(char c){
   if(c == 'u'){
      move(racket_bot_row, racket_col);
      addstr(BLANK);
      racket_top_row -= 1;
      racket_bot_row -= 1;
      move(racket_top_row, racket_col);
      addch('|');
      refresh();
   }
   else if(c == 'd'){
      move(racket_top_row, racket_col);
      addstr(BLANK);
      racket_top_row += 1;
      racket_bot_row += 1;
      move(racket_bot_row, racket_col);
      addch('|');
      refresh();
   }
}

void f(int signum){
   endwin();
   exit(1);
}
int main(int argc, char* argv[]){
   int c;
   void move_msg(int);
   void f(int);
   
   initscr();
   crmode();
   noecho();
   clear();
   
   symbol = argv[1][0];
   racket_col = atoi(argv[2]);
   racket_top_row = atoi(argv[3]);
   racket_bot_row = atoi(argv[4]);
   ball_start_col = atoi(argv[5]);
   ball_start_row = atoi(argv[6]);
   top_row = atoi(argv[7]);
   bot_row = atoi(argv[8]);
   l_edge = atoi(argv[9]);
   r_edge = atoi(argv[10]);
   col_dir = -1.0;
   row_dir = 1.0;
   delay = 40;
   
   signal(SIGQUIT, SIG_IGN);
   signal(SIGINT, f);
   curs_set(0);
   set_map();
   set_racket();
   move(ball_start_row, ball_start_col);
   addch(symbol);
   signal(SIGALRM, move_msg);
   set_ticker(delay);
   start=time(NULL);
   while(true){
      ndelay = 0;
      c = getch();
      if(c == 'q') col_dir /= 2.0f;
      if(c == 'w') row_dir /= 2.0f;
      if(c == 'e') col_dir *= 2.0f;
      if(c == 'r') row_dir *= 2.0f;
      if(c == 'u' || c == 'd') move_racket(c);
      if(c == 'Q') break;
      if(ndelay > 0)
         set_ticker(delay=ndelay);
   }
   endwin();
   return 0;
}