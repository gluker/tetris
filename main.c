#include <ncurses.h>
#include <time.h>
#include <stdlib.h>

#define RIGHT_PAN   7
#define FIG_NUM     7
#define DELAY_TIME  1.0
#define FRAMECHAR   '|'
#define FIELD_W     10
#define FIELD_H     20
#define WID         2
#define BLOCKCHAR   "X"
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
    draw_frame();

    figure cf = figs[rand()%(FIG_NUM-1)];
    figure nf = figs[rand()%(FIG_NUM-1)];
    int score = 0;
    int x=3,y=0;
    int ch;
    int game_state = 1;
    double delay = DELAY_TIME;
    while(game_state){
        clock_t start = clock();
        clock_t finish = start+(delay*CLOCKS_PER_SEC);
        while(clock()<finish){
            ch=getch();
            switch (ch){
            case KEY_EXIT: game_state = 0; break;
            case KEY_LEFT: move_fig(field,cf,&x,&y,M_L); break;
            case KEY_RIGHT: move_fig(field,cf,&x,&y,M_R); break;
            case KEY_DOWN: move_fig(field,cf,&x,&y,M_D); break;
            case KEY_UP:    rotate_fig(field,&cf,&x,&y,R_R); break;

            default:
                break;
            }

            draw_field(field);
        }
        if (!move_fig(field,cf,&x,&y,M_D)){
            if(y<=1) game_state = 0;
            x=3;
            y=0;
            cf = nf;
            unprintfig(nf,2,12);
            nf = figs[rand()%(FIG_NUM)];
            printfig(nf,2,12);
            fill_test(field,&score);
            mvprintw(10,25,"%d",score);
            delay-=(double)((score/3)/10);
        }

        draw_field(field);
        refresh();
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
void draw_field(int field[FIELD_H][FIELD_W]){
    for(int i=0;i<FIELD_H;i++)
        for(int j=0;j<FIELD_W;j++){
            delblock(i,j);
            if(field[i][j])
                putblock(i,j);
        }
}

void draw_frame(){
    attron(A_REVERSE);

    for(int i=0;i<FIELD_H+2;i++){
        mvprintw(i,0,BLOCKCHAR);
        mvprintw(i,(FIELD_W)*2+1,BLOCKCHAR);
    }
    for(int i=0;i<FIELD_W;i++){
        mvprintw(0,i+(FIELD_W)*2+2,BLOCKCHAR);
        mvprintw(RIGHT_PAN,i+(FIELD_W)*2+2,BLOCKCHAR);
    }
    for(int i=0;i<=RIGHT_PAN;i++){
        mvprintw(i,FIELD_W*3+2,BLOCKCHAR);
    }


    for(int i=1;i<(FIELD_W)*2+2;i++){
        mvprintw(0,i,BLOCKCHAR);
        mvprintw((FIELD_H+1),i,BLOCKCHAR);
    }
    attroff(A_REVERSE);
}

void putfig(int field[FIELD_H][FIELD_W],figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) field[y+i][x+j] = 1;
}

void printfig(figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) putblock(y+i,x+j);
}
void unprintfig(figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) delblock(y+i,x+j);
}

void delfig(int field[FIELD_H][FIELD_W],figure F,int y,int x){
    for(int i=0;i<F.dim;i++)
        for(int j=0;j<F.dim;j++)
            if (F.fig[i][j]) field[y+i][x+j] = 0;
}
void putblock(int y,int x){
    move(y+1,x*2+1);
    attron(A_REVERSE);
    printw("  ");
    attroff(A_REVERSE);
}
void delblock(int y,int x){
    move(y+1,x*2+1);
    printw("  ");
}

