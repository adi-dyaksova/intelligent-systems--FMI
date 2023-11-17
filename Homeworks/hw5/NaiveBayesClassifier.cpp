#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include<sstream>

using namespace std;
vector<vector<string>> people; 
const int TESTS_CNT = 10;
const int SETS_COUNT = 10;
const double lambda = 1;
const int K = 2; //number of classes
const int A = 3; //number of values for attributes

int setSize;
int repCnt=0;
int demCnt=0;

int rep[3][16];
int dem[3][16];

void readData(string file) {
	ifstream infile;
	infile.open(file);

	if (infile.is_open()) {
		string line;
		while (getline(infile, line)) {

			vector<string> attributes;
			stringstream ss(line);

			while (ss.good()) {
				string substr;
				getline(ss, substr, ',');
				attributes.push_back(substr);
			}
			people.push_back(attributes);
		}
		infile.close();
	}
}

void changeData(int i, int value) {
	if (people[i][0] == "republican") {
		repCnt+=value;
	}
	else {
		demCnt += value;
	}

	int counter = 0;
	while (counter < 16) {
		string val = people[i][counter + 1]; //+1 because first value is republican/ democrat
		if (val == "y") {
			if (people[i][0] == "republican") {
				rep[0][counter]+=value;
			}
			else {
				dem[0][counter] += value;
			}
		}
		else if (val == "n") {
			if (people[i][0] == "republican") {
				rep[1][counter] += value;
			}
			else {
				dem[1][counter] += value;
			}
		}
		else if (val == "?") {
			if (people[i][0] == "republican") {
				rep[2][counter] += value;
			}
			else {
				dem[2][counter] += value;
			}
		}
		counter++;
	}
}

void addDataFromFile() {
	for (int i = 0; i < people.size(); i++)
	{
		changeData(i, 1); //add to rep and dem arrays 
	}
}

int classify(vector<string> person) {
	double logRepProbability = 0;
	double logDemProbability = 0;

	for (int i = 1; i < person.size(); i++)
	{
		int attributeValueInd;
		if (person[i] == "y") {
			attributeValueInd = 0;
		}
		else if (person[i] == "n") {
			attributeValueInd = 1;
		}
		else {
			attributeValueInd = 2;
		}
		//P(x|Ck)
		logRepProbability += log((rep[attributeValueInd][i-1] + lambda) / (repCnt + A * lambda));
		logDemProbability += log((dem[attributeValueInd][i-1] + lambda) / (demCnt + A * lambda));
	}
	//P(Ck)
	logRepProbability += log((repCnt + lambda) / (repCnt + demCnt + K * lambda));
	logDemProbability += log((demCnt + lambda) / (repCnt + demCnt + K * lambda));

	if (logRepProbability >= logDemProbability) {
		return 0; //republican
	}
	else {
		return 1; //democrat
	}
}

bool isCorrect(int predictedClass, int ind) {
	return (predictedClass == 0 && people[ind][0] == "republican")
			|| (predictedClass == 1 && people[ind][0] == "democrat");
}

double getAccuracyPercentage(int testSetStartInd) {
	double correctPredictions = 0;
	int predictedClass = -1; //0=>rep, 1=>dem
	for (int i = 0; i < setSize; i++)
	{
		predictedClass = classify(people[testSetStartInd + i]); //return 0=>rep, 1=>dem
		if (isCorrect(predictedClass, testSetStartInd + i)) {
			correctPredictions++;
		}
	}
	return correctPredictions * 100 / setSize;
}

int main()
{
	readData("house-votes-84.data");
	addDataFromFile();
	
	setSize = people.size() / SETS_COUNT;
	int testSetStartInd;
	double accuracySum = 0;

	for (int j = 0; j < TESTS_CNT; j++)
	{
		testSetStartInd = j * setSize;
		int testSetEndInd = testSetStartInd + setSize;

		for (int i = testSetStartInd; i < testSetEndInd ; i++)
		{
			if (testSetStartInd == 0) { //first test does not have previously removed lines
				changeData(i, -1);
			}
			else {
				changeData(i - setSize, 1); //add values from previous test set
				changeData(i, -1); //remove values from current test set
			}

		}
		double currAccuracy = getAccuracyPercentage(testSetStartInd);
		cout << "Test "<<j+1<<" accuracy: "<< currAccuracy << "%" << endl;
		accuracySum += currAccuracy;
	}

	cout << endl;
	cout << "The average accuracy is "<< accuracySum / TESTS_CNT << "%" << endl;
}
