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
vector<pair<int, int> > place_list;

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

//(x,y)に石を置くことができるか判定する関数
bool Can_Put(int x,int y) {
    if(board[x][y] != 0 || x < 1 || 8 < x || y < 1 || 8 < y) return false;
    bool can_put = false;
    for(int dx=-1;dx<2;dx++) {
        for(int dy=-1;dy<2;dy++) {
            if(dx == 0 && dy == 0) continue;
            int count = 1;
            while(true) {
                //cout << x+dx*count <<  ' ' << y+dy*count << endl;
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
            //cout << i << ' ' << j << endl;
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
    for(int i=0;i<10;i++) {
        for(int j=0;j<10;j++) {
            if(board[i][j] == 0) cout << "-" ;
            if(board[i][j] == 1) cout << "●" ;
            if(board[i][j] == -1) cout << "◯" ;
            if(board[i][j] == 2) {
                if((i == 0 && j == 0) || (i == 0 && j == 9) || (i == 9 && j == 0) || (i == 9 && j == 9)) cout << ' ';
                else if(i == 0 || i == 9) cout << j;
                else cout << i;
            }
        }
        cout << endl;
    }
}

//石を置く処理を実行する関数
void Put_Stone(int x,int y) {
    place_list.push_back(make_pair(x,y));
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
                    place_list.push_back(make_pair(x+dx*i,y+dy*i));
                }
            }
        }
    }
    board[x][y] = player;
}

// 1手前に戻す関数
void Undo_Put_Stone(vector<pair<int, int> > &place_list) { // place_list[0]は新しく石を置いた場所 それ以外は既に置いてあった石を反転させた位置
    int list_size = place_list.size();
    for(int i=0;i<list_size;i++) {
        int x = place_list[i].first, y = place_list[i].second;
        if(i == 0) {
            board[x][y] = 0; //置いた石を取り除く
        } else {
            board[x][y] *= -1; //反転させた石を元に戻す
        }
    }
    place_list.erase(place_list.begin(),place_list.end());
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

//メイン関数(オセロ 対戦ができる)
int main() {
    Make_Board();
    while(Can_Continue() == true) {
        Show_Board();
        sleep(1);
        int x , y;
        if(player == 1) {
            cout << "先手(白)の番です。石を置く場所を入力してください。" << endl;
            for(int i=0;i<10;i++) {
                for(int j=0;j<10;j++) {
                    if(Can_Put(i,j) == true) cout << "(" << i << "," << j << ") ";
                }
            }
            cout << "に石を置くことができます。" << endl;
            cin >> x >> y;
            while(Can_Put(x,y) == false) {
                cin >> x >> y;
            }
        }
        else {
            vector<pair<int,int> > candidate;
            for(int i=0;i<10;i++) {
                for(int j=0;j<10;j++) {
                    if(Can_Put(i,j) == true) candidate.push_back(make_pair(i,j));
                }
            }
            int max_size = candidate.size();
            random_device rand_maker;
            int rand = rand_maker() % max_size;
            x = candidate[rand].first;
            y = candidate[rand].second;
            cout << "後手(黒)は(" << x << ',' << y << ")に石を置きました。" << endl;
        }
        Put_Stone(x,y);
        sleep(1);
        player *= (-1);
    }
    Show_Result();
    return 0;
}