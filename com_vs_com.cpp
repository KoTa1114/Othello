#include <iostream>
#include <vector>
#include <unistd.h>
#include <utility>
#include <random>

using namespace std;

//手番を表す変数
int player = 1;

//盤面の状態を記録する配列
int board[10][10];

//置いた石、反転させた石の位置を保存しておく配列
vector<vector<pair<int, int> > > place_list_white, place_list_black;

//盤面を初期化する関数
void Make_Board() {
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(i == 0 || i == 9 || j == 0 || j == 9) board[i][j] = 2;
            else board[i][j] = 0;
        }
    }
    board[4][4] = 1;
    board[5][5] = 1;
    board[4][5] = -1;
    board[5][4] = -1;
    return;
}

//(x,y)が盤面内かを判定する関数
bool is_in(int x, int y) {
    return (x > 0 && x < 9 && y > 0 && y < 9);
}

//盤面の評価関数
int evaluation_function1[10][10] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,120,-20,20,5,5,20,-20,120,0},
    {0,-20,-40,-5,-5,-5,-5,-40,-20,0},
    {0,20,-5,15,3,3,15,-5,20,0},
    {0,5,-5,3,3,3,3,-5,5,0},
    {0,5,-5,3,3,3,3,-5,5,0},
    {0,20,-5,15,3,3,15,-5,20,0},
    {0,-20,-40,-5,-5,-5,-5,-40,-20,0},
    {0,120,-20,20,5,5,20,-20,120,0},
    {0,0,0,0,0,0,0,0,0,0}
};
// https://niwakomablog.com/othello-algorithm-part5/　より引用

int evaluation_function2[10][10] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0}
};

int evaluation_function_trained[10][10] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,24,1,-54,74,-16,46,33,-68,0},
    {0,-19,1,1,1,1,1,1,-9,0},
    {0,0,1,1,-51,6,32,1,-41,0},
    {0,41,31,1,1,1,1,5,1,0},
    {0,1,-12,20,6,1,32,4,-31,0},
    {0,1,25,11,-40,-22,-52,1,-15,0},
    {0,1,74,-53,-43,-29,81,0,1,0},
    {0,1,1,1,-18,1,-11,3,1,0},
    {0,0,0,0,0,0,0,0,0,0}
};

// 盤面の評価値を計算する関数
int Calculate_Score(int player, int evaluation_function[10][10]) {
    int score_player = 0, score_opponent = 0;
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(board[i][j] == player) {
                score_player += evaluation_function[i][j];
            } else if(board[i][j] == player*(-1)) {
                score_opponent += evaluation_function[i][j];
            }
        }
    }
    return score_player - score_opponent;
}

//(x,y)に石を置くことができるか判定する関数
bool Can_Put(int x,int y) {
    if(board[x][y] != 0 || x < 1 || 8 < x || y < 1 || 8 < y) return false;
    bool can_put = false;
    for(int dx=-1;dx<2;dx++) {
        for(int dy=-1;dy<2;dy++) {
            if(dx == 0 && dy == 0) continue;
            int count = 1;
            while(true) {
                if(board[x+dx*count][y+dy*count] == 2 || board[x+dx*count][y+dy*count] == 0) break;
                if(board[x+dx*count][y+dy*count] == player*(-1)) {
                    count++;
                }
                if(board[x+dx*count][y+dy*count] == player) {
                    if(count > 1) {
                        can_put = true;
                        break;
                    }
                    else {
                        break;
                    }
                }
            }
        }
    }
    return can_put;
}

//ゲームを継続可能か判定する関数
bool Can_Continue() {
    bool can_continue = false;
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j) == true) {
                can_continue = true;
                break;
            }
        }
        if(can_continue == true) break;
    }
    if(can_continue == true) {
        return true;
    }
    player *= -1;
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j) == true) {
                can_continue = true;
                break;
            }
        }
    }
    if(can_continue == true) {
        return true;
    }
    return false;
}

//盤面の状態を表示する関数
void Show_Board() {
    cout << "    a    b    c    d    e    f    g    h" << endl;
    for(int i=1;i<9;i++) {
        cout << "   ---- ---- ---- ---- ---- ---- ---- ----" << endl;
        cout << i << ' ';
        for(int j=1;j<9;j++) {
            cout << "|";
            if(board[i][j] == 1) cout << " ⚫︎ ";
            else if(board[i][j] == -1) cout << " ⚪︎ ";
            else cout << "    ";
        }
        cout << "|" << endl;
    }
    cout << "   ---- ---- ---- ---- ---- ---- ---- ----" << endl;
}

//石を置く処理を実行する関数
void Put_Stone(int x,int y) {
    vector<pair<int, int> > place_list_sub;
    place_list_sub.push_back(make_pair(x,y));
    for(int dx=-1;dx<2;dx++) {
        for(int dy=-1;dy<2;dy++) {
            if(dx == 0 && dy == 0) continue;
            bool can_put = false;
            int count = 1;
            while(true) {
                if(board[x+dx*count][y+dy*count] == 2 || board[x+dx*count][y+dy*count] == 0) break;
                if(board[x+dx*count][y+dy*count] == player*(-1)) {
                    count++;
                }
                if(board[x+dx*count][y+dy*count] == player) {
                    if(count > 1) {
                        can_put = true;
                        break;
                    }
                    else {
                        break;
                    }
                }
            }
            if(can_put == true) {
                for(int i=1;i<count;i++) {
                    board[x+dx*i][y+dy*i] = player;
                    place_list_sub.push_back(make_pair(x+dx*i,y+dy*i));
                }
            }
        }
    }
    board[x][y] = player;
    if(player == 1) {
        place_list_white.push_back(place_list_sub);
    } else if(player == -1) {
        place_list_black.push_back(place_list_sub);
    }
}

// 1手前に戻す関数
void Undo_Put_Stone(vector<vector<pair<int, int> > > &place_list) { // place_list[i][0]は新しく石を置いた場所 それ以外は既に置いてあった石を反転させた位置
    int id = place_list.size()-1;
    if(id < 0) {
        cout << "これ以上戻すことはできません" << endl;
        return;
    }
    int list_size = place_list[id].size();
    for(int i=0;i<list_size;i++) {
        int x = place_list[id][i].first, y = place_list[id][i].second;
        if(is_in(x,y)) {
            if(i == 0) {
                board[x][y] = 0; //置いた石を取り除く
            } else {
                board[x][y] *= (-1); //反転させた石を元に戻す
            }
        }
    }
    place_list.pop_back();
}

//結果を表示する関数
void Show_Result() {
    int count1 = 0, count2 = 0;
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(board[i][j] == 1) {
                count1++;
            }
            if(board[i][j] == -1) {
                count2++;
            }
        }
    }
    cout << "先手(白) : " << count1 << " 個 後手(黒) : " << count2 << "個" << endl; 
    if(count1 > count2) {
        cout << "先手の(白)の勝利です" << endl;
    }
    if(count1 == count2) {
        cout << "引き分けです" << endl;
    }
    if(count1 < count2) {
        cout << "後手(黒)の勝利です" << endl;
    }
}

// 石を置く場所と、評価値をもつ構造体

struct Action {
    int score, x, y;
    Action(int score, int x, int y) {
        this->score = score;
        this->x = x;
        this->y = y;
    }
};

const int initial_x = -100, initial_y = -100;

// 石をおける場所をランダムに選択して返す
Action Make_Random_Action() {
    Action action(-1, initial_x, initial_y);
    vector<pair<int,int> > candidate; //石を置ける場所をもつ配列
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j) == true) {
                candidate.push_back(make_pair(i,j));
            }
        }
    }
    if(candidate.size() > 0) {
        random_device rand_maker;
        int rand = rand_maker() % candidate.size();
        action.x = candidate[rand].first;
        action.y = candidate[rand].second;
    }
    return action;
}

Action Mini_Max(int depth, int x, int y, int player_sub, int mini_max_depth, int evaluation_function[10][10]) {
    //葉に到達した場合は評価値を計算する
    if(depth == 0) {
        int score = Calculate_Score(player_sub,evaluation_function);
        Action action(score,x,y);
        return action;
    }
    Action action(0,x,y);
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j)) {
                Put_Stone(i,j);
                player *= (-1);
                Action action_sub = Mini_Max(depth-1,i,j,player_sub,mini_max_depth,evaluation_function);
                player *= (-1);
                if(player == 1) Undo_Put_Stone(place_list_white);
                else if(player == -1) Undo_Put_Stone(place_list_black);
                if(player == player_sub*(-1)) {
                    if(action.score > action_sub.score) {
                        action.score = action_sub.score;
                        if(depth == mini_max_depth) {
                            action.x = action_sub.x;
                            action.y = action_sub.y;
                        }
                    }
                } else if(player == player_sub) {
                    if(action.score < action_sub.score) {
                        action.score = action_sub.score;
                        if(depth == mini_max_depth) {
                            action.x = action_sub.x;
                            action.y = action_sub.y;
                        }
                    }
                }
            }
        }
    }
    if(action.score == 0) {
        action = Make_Random_Action();
    }
    return action;
}

//alpha := 自分の番で最大の評価値  beta := 相手の番で最小の評価値
// 相手の番でalphaより大きいと探索は打ち切り　自分の番でbetaより小さいと探索は打ち切り

Action Alpha_Beta(int depth, int x, int y, int alpha, int beta, int player_sub, int alpha_beta_depth, int evaluation_function[10][10] ) {
    //葉に到達した場合は評価値を計算する
    if(depth == 0) {
        int score = Calculate_Score(player_sub,evaluation_function);
        Action action(score,x,y);
        return action;
    }
    Action action(0,x,y);
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j)) {
                Put_Stone(i,j);
                player *= (-1);
                Action action_sub = Alpha_Beta(depth-1,i,j,alpha,beta,player_sub,alpha_beta_depth,evaluation_function);
                player *= (-1);
                if(player == 1) Undo_Put_Stone(place_list_white);
                else if(player == -1) Undo_Put_Stone(place_list_black);
                if(player == player_sub*(-1)) {
                    beta = min(beta,action_sub.score);
                    if(action_sub.score < alpha) {
                        i = 10;
                        j = 10;
                    }
                    if(action.score > action_sub.score) {
                        action.score = action_sub.score;
                        if(depth == alpha_beta_depth) {
                            action.x = action_sub.x;
                            action.y = action_sub.y;
                        }
                    }
                } else if(player == player_sub) {
                    alpha = max(alpha,action_sub.score);
                    if(action_sub.score > beta) {
                        i = 10;
                        j = 10;
                    }
                    if(action.score < action_sub.score) {
                        action.score = action_sub.score;
                        if(depth == alpha_beta_depth) {
                            action.x = action_sub.x;
                            action.y = action_sub.y;
                        }
                    }
                }
            }
        }
    }
    if(action.score == 0) {
        action = Make_Random_Action();
    }
    return action; 
}

int victory_white = 0, victory_black = 0;
void Do_Game() {
    place_list_white.clear();
    place_list_black.clear();
    player = 1;
    Make_Board();
    while(Can_Continue() == true) {
        //Show_Board();
        int x = initial_x , y = initial_y;
        Action action(-1,x,y);
        if(player == 1) {
            //action = Make_Random_Action();
            int depth1 = 1;
            action = Alpha_Beta(depth1,-1,-1,-1e9,1e9,player,depth1,evaluation_function2);
            //action = Mini_Max(depth1,-1,-1,player,depth1);
        } else if(player == -1) {
            int depth2 = 1;
            //action = Alpha_Beta(depth2,-1,-1,-1e9,1e9,player,depth2,evaluation_function1);
            action = Make_Random_Action();
            //action = Mini_Max(depth2,-1,-1,player,depth2);
        }
        x = action.x;
        y = action.y;
        if(x != initial_x && y != initial_y) {
            Put_Stone(x,y);
        }
        else {
            if(player == 1) {
                place_list_white.push_back({{-1,-1}});
            } else if(player == -1) {
                place_list_black.push_back({{-1,-1}});
            }
        }
        player *= (-1);
    }
    //Show_Board();
    int cnt_w = 0, cnt_b = 0;
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(board[i][j] == 1) cnt_w++;
            else if(board[i][j] == -1) cnt_b++;
        }
    }
    if(cnt_w > cnt_b) victory_white++;
    else if(cnt_w < cnt_b) victory_black++;
}

int victory = 0;

//山登り法 評価関数を改善
void Hill_Climbing(int evaluation_function[10][10]) {
    random_device rd;
    default_random_engine eng(rd());
    uniform_int_distribution<int> distr(1,8);
    vector<Action> pre_eva_list;
    for(int i=0;i<4;i++) {
        int x = distr(eng), y = distr(eng), eva = rd()%50;
        if(rd()%2 == 0) eva *= -1;
        Action pre_action(evaluation_function[x][y],x,y);
        pre_eva_list.push_back(pre_action);
        evaluation_function[x][y] += eva;
    }
    for(int i=0;i<200;i++) {
        Do_Game();
    }
    //勝利数が今までの最大値より小さかった場合は変更を無効にする
    if(victory >= victory_white) {
        for(int i=0;i<4;i++) {
            Action action = pre_eva_list[i];
            evaluation_function[action.x][action.y] = action.score;
        }
    } else {
        victory = victory_white;
    }
    cout << victory << ' ';
    victory_white = 0;
    victory_black = 0;
}

void Show_Evaluation_Function(int evaluation_function[10][10]) {
    for(int i=0;i<10;i++) {
        cout << "{";
        for(int j=0;j<10;j++) {
            cout << evaluation_function[i][j];
            if(j != 9) cout << ",";
        }
        if(i != 9) cout << "}," << endl;
        else cout << "}" << endl;
    }
}

//メイン関数(オセロ 対戦ができる)
int main() {
   /*for(int i=0;i<100;i++) {Do_Game(); cout << i << endl;}
   cout << "w=" << victory_white << ' ' << "b=" << victory_black << endl;*/
   for(int i=0;i<1000;i++) {Hill_Climbing(evaluation_function2); cout << i << endl;}
   Show_Evaluation_Function(evaluation_function2);
    return 0;
}