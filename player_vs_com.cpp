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
int evaluation_function[10][10] = {
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


// 盤面の評価値を計算する関数
int Calculate_Score(int player) {
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

const int mini_max_depth = 3; //探索する深さ(偶数)

Action Mini_Max(int depth, int x, int y) {
    //葉に到達した場合は評価値を計算する
    if(depth == 0) {
        int score = Calculate_Score(-1); //黒をAIにする
        Action action(score,x,y);
        return action;
    }
    Action action(0,x,y);
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j)) {
                Put_Stone(i,j);
                player *= (-1);
                Action action_sub = Mini_Max(depth-1,i,j);
                player *= (-1);
                if(player == 1) Undo_Put_Stone(place_list_white);
                else if(player == -1) Undo_Put_Stone(place_list_black);
                if(player == 1) {
                    if(action.score > action_sub.score) {
                        action.score = action_sub.score;
                        if(depth == mini_max_depth) {
                            action.x = action_sub.x;
                            action.y = action_sub.y;
                        }
                    }
                } else if(player == -1) {
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
    return action;
}

int alpha_beta_depth = 1;
//alpha := 自分の番で最大の評価値  beta := 相手の番で最小の評価値
// 相手の番でalphaより大きいと探索は打ち切り　自分の番でbetaより小さいと探索は打ち切り

Action Alpha_Beta(int depth, int x, int y, int alpha, int beta) {
    //葉に到達した場合は評価値を計算する
    if(depth == 0) {
        int score = Calculate_Score(-1); //黒をAIにする
        Action action(score,x,y);
        return action;
    }
    Action action(0,x,y);
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(Can_Put(i,j)) {
                Put_Stone(i,j);
                player *= (-1);
                Action action_sub = Alpha_Beta(depth-1,i,j,alpha,beta);
                player *= (-1);
                if(player == 1) Undo_Put_Stone(place_list_white);
                else if(player == -1) Undo_Put_Stone(place_list_black);
                if(player == 1) {
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
                } else if(player == -1) {
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
    return action; 
}

//メイン関数(オセロ 対戦ができる)
int main() {
    Make_Board();
    while(Can_Continue() == true) {
        Show_Board();
        sleep(1);
        int x , y;
        vector<pair<int, int> > candidate; // 石をおける場所をもつ配列
        if(player == 1) {
            cout << "先手(白)の番です。石を置く場所を入力してください。" << endl;
            for(int i=0;i<10;i++) {
                for(int j=0;j<10;j++) {
                    if(Can_Put(i,j) == true) {
                        candidate.push_back(make_pair(i,j));
                    }
                }
            }
            for(auto cand : candidate) {
                int i = cand.first, j = cand.second;
                cout << "(" << i << "," << j << ") ";
            }
            if(candidate.size() > 0) cout << "に石を置くことができます。" << endl;
            if(candidate.size() > 0) {
                random_device rand_maker;
                int rand = rand_maker() % candidate.size();
                x = candidate[rand].first;
                y = candidate[rand].second;
            }
            else {
                cout << "先手(白)は石を置くことができません" << endl;
                place_list_white.push_back({{-1,-1}});
            }
            cout << "-1 -1を入力することで1手前に戻すことができます" << endl;
            cin >> x >> y;
            if(x == -1 && y == -1) {
                Undo_Put_Stone(place_list_black);
                Undo_Put_Stone(place_list_white);
                continue;
            }
            while(Can_Put(x,y) == false) {
                cin >> x >> y;
            }
        }
        else {
            for(int i=0;i<10;i++) {
                for(int j=0;j<10;j++) {
                    if(Can_Put(i,j) == true) candidate.push_back(make_pair(i,j));
                }
            }
            int max_size = candidate.size();
            if(max_size > 0) {
                random_device rand_maker;
                int rand = rand_maker() % max_size;
                x = candidate[rand].first;
                y = candidate[rand].second;
                //Action action = Mini_Max(mini_max_depth,-1,-1);
                Action action = Alpha_Beta(alpha_beta_depth,-1,-1,-1e9,1e9);
                if(action.score != 0) {
                    x = action.x;
                    y = action.y;
                }
                cout << "後手(黒)は(" << x << ',' << y << ")に石を置きました。" << endl;
            } else {
                cout << "後手(黒)は石を置くことができません" << endl;
                place_list_black.push_back({{-1,-1}});
            }
        }
        Put_Stone(x,y);
        sleep(1);
        player *= (-1);
    }
    Show_Result();
    return 0;
}