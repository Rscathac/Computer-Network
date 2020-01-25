#include<iostream>
#include<stdio.h>

using namespace std;

int tic_tac_toe(int board[9], int chess) {

	char symb[3] = {' ', 'O', 'X'};

	int ret = 0;
    int win[3] = {-1, -1, -1};

	if(board[0] == 1 && board[4] == 1 && board[8] == 1) {
        win[0] = 0;
        win[1] = 4;
        win[2] = 8;
		ret = 1;
	}
    else if(board[0] == 2 && board[4] == 2 && board[8] == 2) {
        win[0] = 0;
        win[1] = 4;
        win[2] = 8;
		ret = 2;
    }
    else if(board[0] == 1 && board[1] == 1 && board[2] == 1) {
        win[0] = 0;
        win[1] = 1;
        win[2] = 2;
		ret = 1;
    }
	
    else if(board[0] == 2 && board[1] == 2 && board[2] == 2) {
        win[0] = 0;
        win[1] = 1;
        win[2] = 2;
        ret = 2;
    }
    else if(board[3] == 1 && board[4] == 1 && board[5] == 1) {
        win[0] = 3;
        win[1] = 4;
        win[2] = 5;
		ret = 1;
    }
    else if(board[3] == 2 && board[4] == 2 && board[5] == 2) {
        win[0] = 3;
        win[1] = 4;
        win[2] = 5;
        ret = 2;
    }
    else if(board[6] == 1 && board[7] == 1 && board[8] == 1) {
        win[0] = 6;
        win[1] = 7;
        win[2] = 8;
        ret = 1;
    }
    else if(board[6] == 2 && board[7] == 2 && board[8] == 2) {
        win[0] = 6;
        win[1] = 7;
        win[2] = 8;
		ret = 2;
    }
    else if(board[0] == 1 && board[3] == 1 && board[6] == 1) {
        win[0] = 0;
        win[1] = 3;
        win[2] = 6;
        ret = 1;
    }
    else if(board[0] == 2 && board[3] == 2 && board[6] == 2) {
        win[0] = 0;
        win[1] = 3;
        win[2] = 6;
        ret = 2;
    }
    else if(board[1] == 1 && board[4] == 1 && board[7] == 1) {
        win[0] = 1;
        win[1] = 4;
        win[2] = 7;
        ret = 1;
    }
    else if(board[1] == 2 && board[4] == 2 && board[7] == 2) {
        win[0] = 1;
        win[1] = 4;
        win[2] = 7;
        ret = 2;
    }
    else if(board[2] == 1 && board[5] == 1 && board[8] == 1) {
        win[0] = 2;
        win[1] = 5;
        win[2] = 8;
        ret = 1;
    }
    else if(board[2] == 2 && board[5] == 2 && board[8] == 2) {
        win[0] = 2;
        win[1] = 5;
        win[2] = 8;
        ret = 2;
    }
    else if(board[2] == 1 && board[4] == 1 && board[6] == 1) {
        win[0] = 2;
        win[1] = 4;
        win[2] = 6;
        ret = 1;
    }
    else if(board[2] == 2 && board[4] == 2 && board[6] == 2) {
        win[0] = 2;
        win[1] = 4;
        win[2] = 6;
        ret = 2;
    }
	else 
		ret = 0;

	if(ret == 0) {
		ret = -1;
		for(int i = 0; i < 9; i++) {
			if(board[i] == 0) {
				ret = 0;
				break;
			}
		}		
	}

	for(int i = 0; i < 3; i++) {
		cout << "+-----------+\n";
		for(int j = 0; j < 3; j++) {
			if(j == 0)
				cout << "|";
			if((3*i+j) == win[0] || (3*i+j) == win[1] ||( 3*i+j) == win[2]) {
				if((3*i+j) == chess)
					printf(" \033[5;31;49m%c\033[0m |", symb[board[3*i+j]]);
				else
					printf(" \033[0;31;49m%c\033[0m |", symb[board[3*i+j]]);
			}

			else if((3*i+j) == chess)
					printf(" \033[5;29;49m%c\033[0m |", symb[board[3*i+j]]);
			else
				cout << " " << symb[board[3*i+j]] << " |" ;
			if(j == 2)
				cout << "\n";
		}
	}
	cout << "+-----------+\n";
	
	return ret;

}


