#include <iostream>
#include <vector>
#include <random>
#include <chrono>
using namespace std::chrono;
using namespace std;
int n;
const int K = 2;
random_device rd;
mt19937 gen(rd());

int random(int low, int high)
{
	uniform_int_distribution<> dist(low, high);
	return dist(gen);
}

int getColWithQueenWithMaxConf(int* queens, int* queensPerRow, int* queensPerD1, int* queensPerD2) {
	int* queensConfl = new int[n]; //ind-col,val-number confl
	int* maxConflQueens = new int [n] {0}; 
	int maxConflQueensSize = 0;

	int row;
	int maxNumberConfl = 0;
	for (int col = 0; col < n; col++)
	{
		row = queens[col];
		queensConfl[col] = (queensPerRow[queens[col]] - 1) + (queensPerD1[col - row + n - 1] - 1) + (queensPerD2[col + row] - 1);
		if (queensConfl[col] > maxNumberConfl) {
			maxNumberConfl = queensConfl[col];

			for (int i = 0; i < maxConflQueensSize; i++) //clear
			{
				maxConflQueens[i] = 0;
			}
			maxConflQueensSize = 1;
			maxConflQueens[maxConflQueensSize - 1] = col;

		}
		else if (queensConfl[col] == maxNumberConfl) {
			maxConflQueensSize++;
			maxConflQueens[maxConflQueensSize - 1] = col;

		}
	}

	if (maxNumberConfl != 0) {
		int  ind = maxConflQueens[random(0, maxConflQueensSize - 1)];
		delete[] maxConflQueens;
		return ind;
	}
	else {
		delete[] maxConflQueens;
		return -1;
	}
}

int getRowWithMinConflict(int* queens, int col, int* queensPerRow, int* queensPerD1, int* queensPerD2) {
	int currNumberConfl;
	int minNumberConfl = INT_MAX;
	int* rowsWithMinConflicts = new int [n] {0};
	int rowsWithMinConflictsSize = 0;

	for (int row = 0; row < n; row++)
	{
		if (queens[col] == row) { //current place of the queen
			currNumberConfl = (queensPerRow[queens[col]] - 1) + (queensPerD1[col - row + n - 1] - 1) + (queensPerD2[col + row] - 1);
		}
		else {
			currNumberConfl = queensPerRow[row] + queensPerD1[col - row + n - 1] + queensPerD2[col + row];
		}
		if (currNumberConfl < minNumberConfl) {
			minNumberConfl = currNumberConfl;
			for (int i = 0; i < rowsWithMinConflictsSize; i++) //clear
			{
				rowsWithMinConflicts[i] = 0;
			}
			rowsWithMinConflictsSize = 1;
			rowsWithMinConflicts[rowsWithMinConflictsSize - 1] = row;
		}
		else if (currNumberConfl == minNumberConfl) {
			rowsWithMinConflictsSize++;
			rowsWithMinConflicts[rowsWithMinConflictsSize - 1] = row;
		}
	}	
	int  ind = rowsWithMinConflicts[random(0, rowsWithMinConflictsSize - 1)];
	delete[] rowsWithMinConflicts;
	return ind;
}

void moveQueen(int* queens, int newRow, int col, int* queensPerRow, int* queensPerD1, int* queensPerD2) {

	int oldRow = queens[col];

	//decrease number of queens in the old place
	queensPerRow[queens[col]]--;
	queensPerD1[col - oldRow + n - 1]--;
	queensPerD2[col + oldRow]--;

	//increase number of queens in the new place
	queens[col] = newRow;
	queensPerRow[queens[col]]++;
	queensPerD1[col - newRow + n - 1]++;
	queensPerD2[col + newRow]++;
}

void printResult(int* queens) {
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (queens[j] == i) {
				cout << " *";
			}
			else {
				cout << " _";
			}
		}
		cout << endl;
	}

}

void initialize(int* queens, int* queensPerRow, int* queensPerD1, int* queensPerD2) {

	//RANDOM INITILOIZATION - around 350 milliseconds for n = 10000
	//								
	//int row;
	//for (int col = 0; col < n; col++)
	//{
	//	queens[col] = random(0, n - 1);
	//	row = queens[col];
	//	queensPerRow[queens[col]]++;
	//	queensPerD1[col - row + n - 1]++;
	//	queensPerD2[col + row]++;
	//}

	// HORSE INITILOIZATION - around 10 milliseconds for n = 1 000 000
	int row = 0;
	int iter = 1;
	for (int col = 0; col < n; col++)
	{
		if (col % 2 == 0) {
			row = n / 2 + iter - 1;
		}
		else {
			row = iter-1; 
			iter++;
		}
		queens[col] = row;
		queensPerRow[row]++;
		queensPerD1[col - row + n - 1]++;
		queensPerD2[col + row]++;
	}
}


void solve(int* queens, int* queensPerRow, int* queensPerD1, int* queensPerD2) {
	int iter = 0;
	int colQueenMaxConfl=-2;
	int rowMinConfl;
	bool hasConflicts = true;

	while (iter++ <= K * n) {
		colQueenMaxConfl = getColWithQueenWithMaxConf(queens, queensPerRow, queensPerD1, queensPerD2);
		if (colQueenMaxConfl == -1) { //no conflicts found
			return;
		}
		rowMinConfl = getRowWithMinConflict(queens, colQueenMaxConfl, queensPerRow, queensPerD1, queensPerD2);
		moveQueen(queens, rowMinConfl, colQueenMaxConfl, queensPerRow, queensPerD1, queensPerD2);
	}
	if (colQueenMaxConfl != -1) {
		solve(queens, queensPerRow, queensPerD1, queensPerD2);
	}
}

int main() {

	cin >> n;
	if (n <=0 ) {
		cout << "N should have a positive value." << endl;
	}
	else if (n == 1) {
		cout << "*" << endl;
	}
	else if (n == 2 || n == 3) {
		cout<<"There is not a solution."<<endl;
	}
	else {
		int* queens = new int[n];
		int* queensPerRow = new int[n] {0};
		int* queensPerD1 = new int[2 * n - 1]{ 0 };
		int* queensPerD2 = new int[2 * n - 1]{ 0 };

		auto start = high_resolution_clock::now();
		initialize(queens, queensPerRow, queensPerD1, queensPerD2);
		solve(queens, queensPerRow, queensPerD1, queensPerD2);
		auto end = high_resolution_clock::now();

		if (n <= 30) {
			printResult(queens);
		}
		auto duration = duration_cast<milliseconds>(end - start);
		cout<<"Time for execution (milliseconds): " << duration.count() << endl;

		delete[] queens;
		delete[] queensPerRow;
		delete[] queensPerD1;
		delete[] queensPerD2;
	}
	
	return 0;
}