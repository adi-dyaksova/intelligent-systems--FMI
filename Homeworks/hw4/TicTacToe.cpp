#include <iostream>
#include <algorithm>
using namespace std;
int board[3][3] = {}; //0- empty, 1-x, 2-o
int invalidValue = -100;
int compRow = -1;
int compCol = -1;
bool isPersonFirst;


void printBoard() {
	cout << " --------- " << endl;
	for (int i = 0; i < 3; i++){
		cout << "|";
		for (int j = 0; j < 3; j++){
			if (board[i][j] == 1) {
				cout << (isPersonFirst ? " x " : " o ");
			}
			else if (board[i][j] == 2) {
				cout << (isPersonFirst ? " o " : " x ");
			}
			else {
				cout << " _ ";
			}
		}
		cout << "|" << endl;
	}
	cout << " --------- " << endl;
}
int checkForWinner(int depth) { 
	int result = 0;
	for (int i = 0; i < 3; i++)
	{
		//check for row winner
			if (board[i][0] == board[i][1]  && board[i][1] == board[i][2] ) {
				if (board[i][0] == 2) { 
					result = 10 - depth; 
					break;
				}
				else if (board[i][0] == 1) { 
					result = -10 + depth;
					break;
				}
			}
		//check for column winner
			if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
				if (board[0][i] == 2) { 
					result = 10 - depth;
					break;
				}
				else if (board[0][i] == 1) {
					result = -10 + depth;
					break;
				}
			}
	}
	if (result == 0) { // if no row or column winner found
		//check for diagonal winner
		if ((board[0][0] == board[1][1] && board[1][1] == board[2][2]) || (board[2][0] == board[1][1] && board[1][1] == board[0][2])) {
			if (board[1][1] == 2) { 
				result = 10 - depth; 
			}
			else if (board[1][1] == 1) { 
				result = -10 + depth;
			}
		}
	}
	return result;
}

bool isCellVacant(int row, int col) {
	return board[row-1][col-1] == 0;
}

void checkInput(int& input,int start, int end) {
	while (!cin.good() || input<start || input > end) {
		cout << "Not valid input. Enter number between "<<start<<" and "<<end<<"." << endl;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cin >> input;
	}
}

void makeTurn() {
	int row = -1;
	int col = -1;
	cout << "Enter row [1-3]: ";
	cin >> row;
	checkInput(row, 1, 3);
	cout << "Enter column [1-3]: ";
	cin >> col;
	checkInput(col, 1, 3);
	while (!isCellVacant(row, col)) {
		cout << "This cell is already occupied. Try again." << endl;
		cout << "Enter row [1-3]: ";
		cin >> row;
		checkInput(row, 1, 3);
		cout << "Enter column [1-3]: ";
		cin >> col;
		checkInput(col, 1, 3);
	}
	board[row - 1][col - 1] = 1;
}

bool areEmptyCells() {
	for (int row= 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			if (board[row][col] == 0)
			{
				return true;
			}
		}
	}
	return false;
}

int terminalTest(int depth) {
	int currentScore = checkForWinner(depth);
	if (currentScore != 0) {
		return currentScore; //winner found
	}
	if (!areEmptyCells()) {
		return 0; //tied
	}
	return invalidValue; 
}

int maxValue(int, int, int);
int minValue( int depth,int a, int b) {
	int value = terminalTest(depth);
	if ( value != invalidValue) { //termination state
		return value;
	}
	else {
		int best = INT_MAX;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				if (board[row][col] == 0) {
					board[row][col] = 1; 
					best = min(best, maxValue(depth + 1, a, b));
					board[row][col] = 0;

					//alpha-beta pruning
					if (best <= a) {
						return best;
					}
					b = min(b, best);
				}
			}
		}
		return best;
	}
}

int maxValue(int depth, int a, int b) {
	int value = terminalTest(depth);
	if (value != invalidValue) { //termination state
		return value;
	}
	else {
		int best = INT_MIN;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				if (board[row][col] == 0) {
					board[row][col] = 2;
					best = max(best, minValue(depth + 1, a, b));
					board[row][col] = 0; 

					//alpha-beta pruning
					if (best >= b) {
						return best;
					}
					a = max(a, best);
				}
			}
		}
		return best;
	}
}

void minimaxDecision() {
	int best = INT_MIN;
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			if (board[row][col] == 0) {
				board[row][col] = 2; 
				int curr = minValue(0, INT_MIN,INT_MAX);
				board[row][col] = 0; 
				if (curr > best) {
					best = curr;
					compRow = row;
					compCol = col;
				}
			}
		}
	}
}

int main() {

	int turn;
	cout << "Do you want to play first?" << endl << "1 = Yes" << endl << "0 = NO" << endl;
	cin >> turn;
	isPersonFirst = turn == 1;
	checkInput(turn, 0, 1);
	
	printBoard();
	
	int emptyCellsCount = 9;
	int row, col;
	int winner = 0;
	while (emptyCellsCount != 0 && winner == 0)
	{
		if (turn % 2 != 0) {
			makeTurn(); //person
		}
		else {
			minimaxDecision();
			board[compRow][compCol] = 2;
		}
		turn++;
		emptyCellsCount--;
		winner = checkForWinner(0);
		printBoard();
	}
	if (winner > 0) {
		cout << "Sorry, you lost. Next time it'll be better!" << endl;
	}
	else if (winner < 0) {
		cout << "Congratulations! You beat the computer!" << endl;
	}

	return 0;
}