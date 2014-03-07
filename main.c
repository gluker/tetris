#include <ncurses.h>
#include <time.h>
#include <stdlib.h>

//TODO: speeding up
//TODO: colors
//TODO: records


#define NF_X        1
#define NF_Y        0
#define RIGHT_PAN   7
#define FIG_NUM     7
#define DELAY_TIME  1.0
#define FIELD_W     10
#define FIELD_H     20
#define MAINWIN_W   22
#define MAINWIN_H   22
#define MAINWIN_X   15
#define MAINWIN_Y   0
#define AUXWIN_W    12
#define AUXWIN_H    6

#define R_L         0
#define R_R         1
#define M_L         0
#define M_R         1
#define M_D         2

struct Figure{
    int dim;
    int fig[4][4];
};
typedef struct Figure figure;

int freeplace();
void rotateaux();
void putblock();
void delblock();
void printfig();
void unprintfig();
void delfig();
void putfig();
void draw_field();
void rotate_fig();
void draw_frame();
int move_fig();
void fill_test();

int main()
{
    initscr();
    curs_set(0);
    noecho();
    nodelay(stdscr,1);
    keypad(stdscr, TRUE);
    srand(time(NULL));


    figure I = {0};
        I.dim = 4;
        I.fig[0][1] = 1;
        I.fig[1][1] = 1;
        I.fig[2][1] = 1;
        I.fig[3][1] = 1;
    figure O = {0};
        O.dim = 2;
        O.fig[0][0] = 1;
        O.fig[1][1] = 1;
        O.fig[0][1] = 1;
        O.fig[1][0] = 1;
    figure J = {0};
        J.dim = 3;
        J.fig[0][0] = 1;
        J.fig[1][0] = 1;
        J.fig[1][1] = 1;
        J.fig[1][2] = 1;
    figure L = {0};
        L.dim = 3;
        L.fig[2][0] = 1;
        L.fig[1][0] = 1;
        L.fig[1][1] = 1;
        L.fig[1][2] = 1;
    figure S = {0};
        S.dim = 3;
        S.fig[0][0] = 1;
        S.fig[1][0] = 1;
        S.fig[1][1] = 1;
        S.fig[2][1] = 1;
    figure Z = {0};
        Z.dim = 3;
        Z.fig[0][1] = 1;
        Z.fig[1][0] = 1;
        Z.fig[1][1] = 1;
        Z.fig[2][0] = 1;
    figure T = {0};
        T.dim = 3;
        T.fig[0][1] = 1;
        T.fig[1][0] = 1;
        T.fig[1][1] = 1;
        T.fig[1][2] = 1;
    figure figs[] = {I,O,J,L,S,Z,T};
    int field[FIELD_H][FIELD_W] = {{0}};

    WINDOW *main_win = newwin(MAINWIN_H,MAINWIN_W,MAINWIN_Y,MAINWIN_X);
	box(main_win, 0 , 0);
    WINDOW *aux_win = newwin(AUXWIN_H,AUXWIN_W, MAINWIN_Y,MAINWIN_X+MAINWIN_W);
    box(aux_win,0,0);
    nodelay(main_win,1);
    keypad(main_win, TRUE);

    figure cf = figs[rand()%(FIG_NUM-1)];
    figure nf = figs[rand()%(FIG_NUM-1)];
    printfig(aux_win,nf,NF_Y,NF_X);
    int score = 0;
    int x=3,y=-1;
    int ch;
    int game_state = 1;
    double delay = DELAY_TIME;

    while(game_state){

        if (!move_fig(field,cf,&x,&y,M_D)){
            if(y<0) game_state = 0;
            x=3;
            y=-1;
            cf = nf;
            unprintfig(aux_win,nf,NF_Y,NF_X);
            nf = figs[rand()%(FIG_NUM)];
            printfig(aux_win,nf,NF_Y,NF_X);
            fill_test(field,&score);
          //  mvprintw(9,23,"Score: %d",score);
            //delay = 1/(score+1);
          //  mvprintw(10,23,"Delay: %f",delay);
        }

        clock_t start = clock();
        clock_t finish = start+(delay*CLOCKS_PER_SEC);

        while(clock()<finish){
            ch=wgetch(main_win);
            switch (ch){
                case KEY_EXIT: game_state = 0; break;
                case KEY_LEFT: move_fig(field,cf,&x,&y,M_L); break;
                case KEY_RIGHT:move_fig(field,cf,&x,&y,M_R); break;
                case KEY_DOWN: move_fig(field,cf,&x,&y,M_D); break;
                case KEY_UP:   rotate_fig(field,&cf,&x,&y,R_R); break;
                default:
                    break;
            }

            draw_field(main_win,field);
         //   wrefresh(main_win);
        }

        draw_field(main_win,field);

    }

    endwin();
    return 0;
}
void rotate_fig(int field[FIELD_H][FIELD_W],figure *F,int* x,int* y,int dir){
    figure tempf = *F;
    delfig(field,*F,*y,*x);
    rotateaux(&tempf,dir);

    if (freeplace(field,tempf,*y,*x)){
        rotateaux(F,dir);
        putfig(field,*F,*y,*x);
    }

    putfig(field,*F,*y,*x);
}
void rotateaux(figure* F,int dir){
    int tempf[F->dim][F->dim];
    for(int i=0;i<F->dim;i++)
        for(int j=0;j<F->dim;j++)
            tempf[i][j] = F->fig[i][j];

     for(int i=0;i<F->dim;i++)
         for(int j=0;j<F->dim;j++)
            F->fig[i][j] = (dir?tempf[F->dim-j-1][i]:tempf[j][F->dim-i-1]);
}
int freeplace(int field[FIELD_H][FIELD_W],figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++){
            if ((F.fig[i][j]) && (field[y+i][x+j]))
                return 0;
            if ((F.fig[i][j]) && ((y+i>=FIELD_H) || (x+j>=FIELD_W) ||(x+j<0) ))
                return 0;
        }
    return 1;
}
int move_fig(int field[FIELD_H][FIELD_W],figure F,int* x,int* y,int dir){

    int nx=*x,ny=*y;
    switch(dir){
        case M_L:nx--;break;
        case M_R:nx++;break;
        case M_D:ny++;break;
        default:      break;
    }
    delfig(field,F,*y,*x);
    if(freeplace(field,F,ny,nx)){
        putfig(field,F,ny,nx);
        *x=nx;
        *y=ny;
        return 1;
    }
    putfig(field,F,*y,*x);
    return 0;
}
int testline(int line[FIELD_W]){
    for(int i=0;i<FIELD_W;i++)
        if(!line[i]) return 0;
    return 1;
}
void delline(int field[FIELD_H][FIELD_W],int line){
    for(int i=0;i<FIELD_W;i++)
        for(int j=line;j>0;j--)
            field[j][i] = field[j-1][i];

}
void fill_test(int field[FIELD_H][FIELD_W],int* score){
    for(int i=0;i<FIELD_H;i++){
        if (testline(field[i])){
            delline(field,i);
            (*score)++;
        }
    }
}
void draw_field(WINDOW *wind,int field[FIELD_H][FIELD_W]){
    for(int i=0;i<FIELD_H;i++)
        for(int j=0;j<FIELD_W;j++){
            delblock(wind,i,j);
            if(field[i][j])
                putblock(wind,i,j);
        }
    wrefresh(wind);
}

void putfig(int field[FIELD_H][FIELD_W],figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) field[y+i][x+j] = 1;
}

void printfig(WINDOW *wind,figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) putblock(wind,y+i,x+j);
    wrefresh(wind);
}
void unprintfig(WINDOW *wind,figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) delblock(wind,y+i,x+j);
    wrefresh(wind);
}

void delfig(int field[FIELD_H][FIELD_W],figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) field[y+i][x+j] = 0;
}
void putblock(WINDOW *wind, int y,int x){
    //move(y+1,x*2+1);
    wattron(wind,A_REVERSE);
    mvwprintw(wind,y+1,x*2+1,"  ");
    wattroff(wind,A_REVERSE);
}
void delblock(WINDOW *wind,int y,int x){
    //move(y+1,x*2+1);
    mvwprintw(wind,y+1,x*2+1,"  ");
}

