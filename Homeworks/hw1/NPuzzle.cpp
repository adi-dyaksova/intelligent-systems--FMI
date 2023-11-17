#include <iostream>
#include <vector>
#include <string>
using namespace std;
int n;
vector<int> tiles;
int goalZeroIndex;
int boardSize;
int currZeroIndex;
const int UP = 1;
const int DOWN = 2;
const int LEFT = 3;
const int RIGHT = 4;
const int FOUND = -10;
const int NOTFOUND = -11;
vector<int> path;

void fillTiles(int n) {
	int tile;
	for (int i = 0; i < n + 1; i++)
	{
		cin >> tile;
		tiles.push_back(tile);
		if (tile == 0) {
			currZeroIndex = i;
		}
	}
}

int getManhattanDistance()
{
	int tempSum = 0;
	int currTileValue;
	int currTileIndex;
	int goalTileX,goalTileY;

	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			currTileIndex = j + i * boardSize;
			currTileValue = tiles[currTileIndex];

			if (currTileValue != 0)
			{
				if (currTileIndex <= goalZeroIndex) { // if the tile is on the left of the blank space, it's goal index is the value-1
					currTileValue = currTileValue - 1;
				}
				goalTileX = currTileValue / boardSize;
				goalTileY = currTileValue % boardSize;
				tempSum += abs(goalTileX - i) + abs(goalTileY - j);
			}
		}
	}

	return tempSum;
}


bool canMove(int direction) {
	if (direction == UP) {
		return currZeroIndex / boardSize != boardSize - 1;
	}
	else if (direction == DOWN) {
		return currZeroIndex / boardSize != 0;
	}
	else if (direction == LEFT) {
		return currZeroIndex % boardSize != boardSize - 1;
	}
	else if (direction == RIGHT) {
		return currZeroIndex % boardSize != 0;
	}
}

void moveTile(int direction) {
	switch (direction)
	{
	case UP:
		swap(tiles[currZeroIndex], tiles[currZeroIndex + boardSize]);
		currZeroIndex = currZeroIndex + boardSize;
		break;
	case DOWN:
		swap(tiles[currZeroIndex], tiles[currZeroIndex - boardSize]);
		currZeroIndex = currZeroIndex - boardSize;
		break;
	case LEFT:
		swap(tiles[currZeroIndex], tiles[currZeroIndex + 1]);
		currZeroIndex = currZeroIndex + 1;
		break;
	case RIGHT:
		swap(tiles[currZeroIndex], tiles[currZeroIndex - 1]);
		currZeroIndex = currZeroIndex - 1;
		break;
	default:
		break;
	}
}


void printPath() {
	cout << path.size() << endl;
	vector<string> moves{ "up", "down", "left", "right" };
	for (int i = 0; i < path.size(); i++) {
		cout << moves[path[i]-1] << endl;
	}
}

int makeMove(int forward, int backward, int& min, int g, int threshold);


int search(int g, int threshold)
{
	int manhattanDistance = getManhattanDistance();
	int f = g + manhattanDistance;
	
	if (f > threshold) //if current state is further away from the goal state, compared to the previous one
	{
		return f;
	}

	if (manhattanDistance == 0) //goal state is reached
	{
		printPath();
		return FOUND;
	}
	int min = INT_MAX; //minimum value of f 
	int temp; //current value of f after making move

	if ((path.size() == 0 || path.back() != UP) && canMove(DOWN))
	{
		temp = makeMove(DOWN, UP, min, g, threshold);
		if (temp == FOUND) return FOUND;
	}

	if ((path.size() == 0 || path.back() != DOWN) && canMove(UP))
	{
		temp = makeMove(UP, DOWN, min, g, threshold);
		if (temp == FOUND) return FOUND;
	}

	if ((path.size() == 0 || path.back() != LEFT) && canMove(RIGHT))
	{
		temp = makeMove(RIGHT, LEFT, min, g, threshold);
		if (temp == FOUND) return FOUND;
	}

	if ((path.size() == 0 || path.back() != RIGHT) && canMove(LEFT))
	{
		temp = makeMove(LEFT, RIGHT, min, g, threshold);
		if (temp == FOUND) return FOUND;
	}
	return min;
}

int makeMove(int forward, int backward, int& min, int g, int threshold) {
	moveTile(forward);
	path.push_back(forward);
	int temp = search(g + 1, threshold);
	if (temp == FOUND)
	{
		return FOUND;
	}
	if (temp < min)
	{
		min = temp;
	}
	path.pop_back();
	moveTile(backward);
	return NOTFOUND;
}


int countInversions()
{
	int inversionsCount = 0;
	for (int i = 0; i < tiles.size()-1; i++) {
		for (int j = i + 1; j < tiles.size(); j++) {
			if (tiles[i]!=0 && tiles[j]!=0 && tiles[i] > tiles[j]) {
				inversionsCount++;
			}
		}
	}
	return inversionsCount;
}

bool isSolvable() {
	int inversions = countInversions();
	if (boardSize % 2 == 0) {
		cout << "currZeroIndex " << currZeroIndex << endl;
		return (inversions + currZeroIndex) % 2 == 1; //when n is even, an n - by - n board is solvable if and only if the number of inversions plus the row of the blank square is odd
	}
	else {
		return inversions % 2 == 0; //when n is odd, an n-by-n board is solvable if and only if its number of inversions is even.
	}
}

void solvePuzzle() {
	if (isSolvable()) {
		int threshold = getManhattanDistance();
		int temp; // result from recursion which ends when a solution is found
		do {
			temp = search(0, threshold); //g=0, h=manhattanDistance
			threshold = temp;
		} while (temp != FOUND);
	}
	else {
		cout << "The puzzle is not solvable." << endl;
	}
	
}

int main()
{
	cin >> n >> goalZeroIndex;
	fillTiles(n);
	boardSize = sqrt(n + 1);

	// zero index default value is bottom right 
	goalZeroIndex = (goalZeroIndex == -1) ? (boardSize* boardSize - 1) : goalZeroIndex;
	solvePuzzle();

	return 0;
}