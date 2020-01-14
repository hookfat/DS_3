#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "algorithm.h"
//#include "board.h"
#include "rules.h"
using namespace std;
//自定義的CELL
class Sandbox_Cell{
    public:
        Sandbox_Cell();
        int orbs_num;
        int capacity;
        char color;
        bool explode;
};
Sandbox_Cell::Sandbox_Cell(){
    orbs_num = 0, capacity = 4, color='w', explode = false;
}
//myboard
class Sandbox{
    public:
        Sandbox();
        Sandbox_Cell cells[ROW][COL];
        void cell_reaction_marker();
        bool cell_is_full(Sandbox_Cell* cell);
        void add_orb(int i, int j, char color);
        void cell_explode(int i, int j);
        void cell_chain_reaction(Player player);
        // The basic functions of the Board
        bool place_orb(int i, int j, Player* player);
        void print_current_board(int i, int j, int round);
        void cell_reset(int i, int j);
        bool win_the_game(Player player);
};
Sandbox::Sandbox(){
    cells[0][0].capacity = 2;
    cells[0][5].capacity = 2;
    cells[4][0].capacity = 2;
    cells[4][5].capacity = 2;
    cells[0][1].capacity = 3;
    cells[0][2].capacity = 3;
    cells[0][3].capacity = 3;
    cells[0][4].capacity = 3;
    cells[1][0].capacity = 3;
    cells[2][0].capacity = 3;
    cells[3][0].capacity = 3;
    cells[1][5].capacity = 3;
    cells[2][5].capacity = 3;
    cells[3][5].capacity = 3;
    cells[4][1].capacity = 3;
    cells[4][2].capacity = 3;
    cells[4][3].capacity = 3;
    cells[4][4].capacity = 3;
}
bool Sandbox::place_orb(int i, int j, Player* player){
    if(index_range_illegal(i, j))
        return false;
    if(cells[i][j].color != player->get_color()){
        if(cells[i][j].color != 'w')
            return false;
    }



    cells[i][j].orbs_num += 1;
    cells[i][j].color = player->get_color();
    if(cell_is_full(&cells[i][j])){
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(*player);
    }
    return true;
}
void Sandbox::cell_reset(int i, int j){
    cells[i][j].orbs_num = 0;
    cells[i][j].explode = false;
    cells[i][j].color = 'w';
}
void Sandbox::cell_explode(int i, int j){

    int orb_num;
    char color = cells[i][j].color;
    cell_reset(i, j);
    if( i + 1 < ROW){
        add_orb(i+1, j, color);
    }
    if( j + 1 < COL){
        add_orb(i, j+1, color);
    }
    if( i - 1 >= 0){
        add_orb(i-1, j, color);
    }
    if( j - 1 >= 0){
        add_orb(i, j-1, color);
    }
}
void Sandbox::add_orb(int i, int j, char color){
    cells[i][j].orbs_num++;
    cells[i][j].color = color;
}
bool Sandbox::cell_is_full(Sandbox_Cell* cell){
    if(cell->orbs_num >= cell->capacity){
        cell->explode = true;
        return true;
    }
    else return false;
}
void Sandbox::cell_reaction_marker(){

    // Mark the next cell whose number of orbs is equal to the capacity
    for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                cell_is_full(&cells[i][j]);
            }
        }
}
void Sandbox::cell_chain_reaction(Player player){

    bool chain_reac = true;

    while(chain_reac){

        chain_reac = false;

        for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                if(cells[i][j].explode){
                    cell_explode(i ,j);
                    chain_reac = true;
                }
            }
        }

        if(win_the_game(player)){
            return;
        }

        cell_reaction_marker();
    }
}
bool Sandbox::win_the_game(Player player){

    char player_color = player.get_color();
    bool win = true;

    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(cells[i][j].color == player_color || cells[i][j].color == 'w') continue;
            else{
                win = false;
                break;
            }
        }
        if(!win) break;
    }
    return win;
}
typedef struct _place_point{
    bool legal = false;
    bool almost_full = false;
    bool lose = false;
}place_point;
typedef struct _Point{
    int x = -1;
    int y = -1;
}Point;
//useful function
void reset_sandbox_B_to_A(Sandbox* A, Sandbox* B);
void set_legal_point(Sandbox* A, place_point* p, char mycolor);
bool look_around(int row, int col, Sandbox* board, char mycolor, int*index);

void algorithm_A(Board board, Player player, int index[]){
    Sandbox sandbox_origin;
    char mycolor = player.get_color();
    char enemycolor;
    if(mycolor == 'r')
        enemycolor = 'b';
    else
        enemycolor = 'r';
    Player player2(enemycolor);

    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            sandbox_origin.cells[i][j].color = board.get_cell_color(i, j);
            sandbox_origin.cells[i][j].orbs_num = board.get_orbs_num(i, j);
        }
    }

    Sandbox sandbox_1;
    Sandbox sandbox_2;
    Sandbox sandbox_3;
    place_point place[ROW][COL];
    //偵測合法的座標 和 快要爆炸的座標
    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            if(sandbox_origin.cells[i][j].color == 'w' || sandbox_origin.cells[i][j].color == mycolor){
                place[i][j].legal = true;
                if(sandbox_origin.cells[i][j].orbs_num == sandbox_origin.cells[i][j].capacity-1)
                    place[i][j].almost_full = true;
            }
        }
    }
    //1 place to win
    cout << "win:" << endl;
    reset_sandbox_B_to_A(&sandbox_1, &sandbox_origin);
    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            if(place[i][j].almost_full){
                sandbox_1.place_orb(i, j, &player);
                if(sandbox_1.win_the_game(player)){
                    index[0] = i;
                    index[1] = j;
                    //cout << i <<','<<j << endl;

                    return;
                }
                reset_sandbox_B_to_A(&sandbox_1, &sandbox_origin);
            }
        }
        index[0] = -1;
        index[1] = -1;
    }
    //2
    cout << "lose:" << endl;
    reset_sandbox_B_to_A(&sandbox_1, &sandbox_origin);
    place_point enemy_place[ROW][COL];
    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            reset_sandbox_B_to_A(&sandbox_1, &sandbox_origin);
            if(place[i][j].legal){
                sandbox_1.place_orb(i, j, &player);
                reset_sandbox_B_to_A(&sandbox_2, &sandbox_1);
                bool kill_flag = true;
                bool win_flag = false;
                for(int k = 0; k<ROW; k++){
                    for(int l = 0; l<COL; l++){
                        if(sandbox_2.place_orb(k, l, &player2)){
                            if(sandbox_2.win_the_game(player2)){
                                place[k][l].lose = true;
                            }
                            else{
                                for(int m = 0; m<ROW; m++){
                                    win_flag = false;
                                    for(int n = 0; n<COL; n++){
                                        reset_sandbox_B_to_A(&sandbox_3, &sandbox_2);
                                        if(sandbox_3.place_orb(m, n, &player)){
                                            if(sandbox_3.win_the_game(player)){
                                                win_flag = true;
                                                break;
                                            }
                                        }
                                    }
                                    if(win_flag)break;
                                }
                            }
                            if(!win_flag)kill_flag = false;
                        }
                        reset_sandbox_B_to_A(&sandbox_2, &sandbox_1);
                    }
                }
                if(kill_flag){
                    index[0] = i;
                    index[1] = j;
                    //cout << "you already die"<<endl;
                    //cout << i<< ',' <<j << endl;
                    //system("pause");
                    return;
                }
            }
        }
    }



    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            if(place[i][j].lose){
                cout << i << ',' << j << endl;
            }
        }
    }
    //system("pause");
    //cin >> index[0] >> index[1];
    //return;

    //defense
    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            if(sandbox_origin.cells[i][j].color == enemycolor){
                if(!look_around(i, j, &sandbox_origin, mycolor, index)){
                    cout <<"defense:"<< i << ',' << j << endl;
                    if(index[0] != -1){
                        //cout <<"defense:"<< i << ',' << j << endl;
                        //system("pause");
                        return;
                    }
                    //system("pause");
                }
            }
        }
    }



    srand(time(NULL)*time(NULL));
    while(1){
        int i = rand() % 5;
        int j = rand() % 6;
        if(place[i][j].legal == true && place[i][j].lose == false){
            index[0] = i;
            index[1] = j;
            return;
        }
    }
}

void reset_sandbox_B_to_A(Sandbox* A, Sandbox* B){
    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            A->cells[i][j].color = B->cells[i][j].color;
            A->cells[i][j].orbs_num = B->cells[i][j].orbs_num;
        }
    }
}
void set_legal_point(Sandbox* A, place_point* p[], char mycolor){
    for(int i = 0; i<ROW; i++){
        for(int j = 0; j<COL; j++){
            if(A->cells[i][j].color == 'w' || A->cells[i][j].color == mycolor){
                p[i][j].legal = true;
                if(A->cells[i][j].orbs_num == A->cells[i][j].capacity-1)
                    p[i][j].almost_full = true;
            }
        }
    }
}
bool look_around(int row, int col, Sandbox* board, char mycolor, int*index){
    int cap = board->cells[row][col].capacity;
    int num = board->cells[row][col].orbs_num;
    bool safe = false;
    Point p[4];
    bool around[4];
    around[0] = true; around[1] = true; around[2] = true; around[3] = true;
    if(num == cap-1){
        if(row+1 < ROW){
            if(board->cells[row+1][col].color == mycolor){
                if(board->cells[row+1][col].orbs_num == board->cells[row+1][col].capacity-1){
                    index[0] = row+1;
                    index[1] = col;
                    return false;
                }
            }
        }
        if(row-1 >= 0){
            if(board->cells[row-1][col].color == mycolor){
                if(board->cells[row-1][col].orbs_num == board->cells[row-1][col].capacity-1){
                    index[0] = row-1;
                    index[1] = col;
                    return false;
                }
            }
        }
        if(col+1 < COL){
            if(board->cells[row][col+1].color == mycolor){
                if(board->cells[row][col+1].orbs_num == board->cells[row][col+1].capacity-1){
                    index[0] = row;
                    index[1] = col+1;
                    return false;
                }
            }
        }
        if(col-1 >= 0){
            if(board->cells[row][col-1].color == mycolor){
                if(board->cells[row][col-1].orbs_num == board->cells[row][col-1].capacity-1){
                    index[0] = row;
                    index[1] = col-1;
                    return false;
                }
            }
        }
    }
    else{
        if(row+1 < ROW){
            if(board->cells[row+1][col].color == mycolor){
                if(board->cells[row+1][col].capacity == cap-1){
                    if(board->cells[row+1][col].orbs_num >= num){
                        safe = true;
                    }
                    else if(board->cells[row+1][col].orbs_num == num-1){
                        p[0].x = row+1;
                        p[0].y = col;
                    }
                }
                else if(board->cells[row+1][col].capacity == cap){
                    if(board->cells[row+1][col].orbs_num > num){
                        safe = true;
                    }
                    else if(board->cells[row+1][col].orbs_num == num){
                        p[0].x = row+1;
                        p[0].y = col;
                    }
                }
            }
            else if(num == 1 && board->cells[row+1][col].capacity == cap-1 && board->cells[row+1][col].color == 'w'){
                p[0].x = row+1;
                p[0].y = col;
                around[0] = false;
            }
        }
        if(row-1 >= 0){
            if(board->cells[row-1][col].color == mycolor){
                if(board->cells[row-1][col].capacity == cap-1){
                    if(board->cells[row-1][col].orbs_num >= num){
                        safe = true;
                    }
                    else if(board->cells[row-1][col].orbs_num == num-1){
                        p[1].x = row-1;
                        p[1].y = col;
                    }
                }
                else if(board->cells[row-1][col].capacity == cap){
                    if(board->cells[row-1][col].orbs_num > num){
                        safe = true;
                    }
                    else if(board->cells[row-1][col].orbs_num == num){
                        p[1].x = row-1;
                        p[1].y = col;
                    }
                }
            }
            else if(num == 1 && board->cells[row-1][col].capacity == cap-1 && board->cells[row-1][col].color == 'w'){
                p[1].x = row-1;
                p[1].y = col;
                around[1] = false;
            }
        }
        if(col+1 < COL){
            if(board->cells[row][col+1].color == mycolor){
                if(board->cells[row][col+1].capacity == cap-1){
                    if(board->cells[row][col+1].orbs_num >= num){
                        safe = true;
                    }
                    else if(board->cells[row][col+1].orbs_num == num-1){
                        p[2].x = row;
                        p[2].y = col+1;
                    }
                }
                else if(board->cells[row][col+1].capacity == cap){
                    if(board->cells[row][col+1].orbs_num > num){
                        safe = true;
                    }
                    else if(board->cells[row][col+1].orbs_num == num){
                        p[2].x = row;
                        p[2].y = col+1;
                    }
                }
            }
            else if(num == 1 && board->cells[row][col+1].capacity == cap-1 && board->cells[row][col+1].color == 'w'){
                p[2].x = row;
                p[2].y = col+1;
                around[2] = false;
            }
        }
        if(col-1 >= 0){
            if(board->cells[row][col-1].color == mycolor){
                if(board->cells[row][col-1].capacity == cap-1){
                    if(board->cells[row][col-1].orbs_num >= num){
                        safe = true;
                    }
                    else if(board->cells[row][col-1].orbs_num == num-1){
                        p[3].x = row;
                        p[3].y = col-1;
                    }
                }
                else if(board->cells[row][col-1].capacity == cap){
                    if(board->cells[row][col-1].orbs_num > num){
                        safe = true;
                    }
                    else if(board->cells[row][col-1].orbs_num == num){
                        p[3].x = row;
                        p[3].y = col-1;
                    }
                }
            }
            else if(num == 1 && board->cells[row][col-1].capacity == cap-1 && board->cells[row][col-1].color == 'w'){
                p[3].x = row;
                p[3].y = col-1;
                around[3] = false;
            }
        }
        if(safe)return true;
        else{
            for(int i = 0; i<4; i++){
                if(p[i].x != -1){
                    index[0] = p[i].x;
                    index[1] = p[i].y;
                    return false;
                }
            }

        }
    }

}

