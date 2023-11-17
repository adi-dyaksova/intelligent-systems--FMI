#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iomanip>
#include <random>
using namespace std;

// Структура, която представя един връх в резултатното decision tree
struct Node
{
	int feature;                                      // Feature-ът(age, menopause, ..), на база на който правим класификация за текущия връх
	string clazz;                                // Този string ще е непразен само за крайни върхове(листа) и ще представя резултатния клас от класификацията(cancer/no cancer)
	unordered_map<string, Node*> children; // Map с върховете наследници на текущия връх. Наследниците са разпределени по стойностите на `feature` (age=10-19, age=20-29, ...)

	// 2 помощни конструктура
	Node(string clazz) : clazz(clazz) {}
	Node(int feature) : feature(feature) {}
	

	// Помощна функция, която добавя връх наследник в списък от наследници
	void addChild(string featureValue, Node* node)
	{
		children[featureValue] = node;
	}
	~Node() {
		for (auto child : children) {
			delete child.second;
		}
	}
};

class DecisionTreeClassifier
{
private:
	int k;                                   // Константа дефинираща минимален брой на обучаващи примери в множеството
	Node* tree;                              // Коренът на резултатното класифициращо дърво
	vector<int> features;               // Списък с всички feature-и на входния dataset(age, menopause, ...)
	unordered_set<string> classes; // Множество със стойностите на всички класове (cancer/no cancer)


	// Рекурсивна функция, която изгражда следващия връх за текущите данни и списък с позволени feature-и
	Node* buildTree(
		vector<unordered_map<int, string> > data,
		vector<string> classifications,
		vector<int> availableFeatures)
	{
		// Крайни условия:
		// 1. Ако размера на данните е по-малък от константата k - искаме да приключим, за да избегнем overfitting
		// 2. Ако сме изчерпали feature-ите, по-които разделяме данните, няма какво повече да правим
		// 3. Ако всички данни от оставащото м-во са от един клас, няма нужда повече да предвиждаме
		if (data.size() < k || availableFeatures.size() == 0 || allHaveSameClass(classifications))
		{
			return new Node(mostCommonClass(classifications));
		}

		// Обхождаме наличните feature-и и търсим този с най-ниска ентропия
		int bestIGFeature = availableFeatures[0];
		int bestIGFeatureIndex = 0;
		double minEntropy = calculateEntropyByFeature(getFeatureValues(data, bestIGFeature), classifications);

		for (int i = 1; i < availableFeatures.size(); i++)
		{
			double currentEntropy = calculateEntropyByFeature(getFeatureValues(data, availableFeatures[i]), classifications);
			if (currentEntropy < minEntropy)
			{
				bestIGFeature = availableFeatures[i];
				bestIGFeatureIndex = i;
				minEntropy = currentEntropy;
			}
		}

		// След като сме го намерили, махаме го от позволените feature-и и започваме да цепим данните спрямо него
		availableFeatures.erase(availableFeatures.begin() + bestIGFeatureIndex);
		unordered_set<string> bestIGFeatureValues = getFeatureValuesSet(data, bestIGFeature);
		Node* node = new Node(bestIGFeature);

		// Обхождаме всички стойности на feature-a. Спрямо тях ще градим нови клони в резултатното decision tree
		for (string featureValue : bestIGFeatureValues)
		{
			// Намираме само данните, които има текущата стойност на feature-а
			vector<unordered_map<int, string> > filteredData;
			vector<string> filteredClassifications;

			for (int i = 0; i < data.size(); i++)
			{
				if (data[i][bestIGFeature] == featureValue)
				{
					filteredData.push_back(data[i]);
					filteredClassifications.push_back(classifications[i]);
				}
			}

			// Извикваме рекурсивно функцията за тези данни, за да построим следващия връх в дървото
			Node* child = buildTree(filteredData, filteredClassifications, availableFeatures);

			// Добавяме следващия връх като наследник на текущия
			node->addChild(featureValue, child);
		}

		return node;
	}

	// Помощна функция, която проверява дали в списъка с класове има само един уникален клас
	bool allHaveSameClass(vector<string> classifications)
	{
		if (classifications.empty())
			return true;
		for (int i = 1; i < classifications.size(); i++)
			if (classifications[i] != classifications[0])
				return false;

		return true;
	}


	// Помощна функция, която връща най-често срещания клас
	string mostCommonClass(vector<string> classifications)
	{
		int maxOccurrences = 0;
		string candidate;
		for (auto current : classifications)
		{
			int occurences = 0;
			for (auto other : classifications)
				if (current == other)
					occurences++;
			if (occurences > maxOccurrences)
				candidate = current;
		}

		return candidate;
	}


	// Помощна функция, която смята ентропията за даден feature по формулата
	double calculateEntropyByFeature(vector<string> featureValues, vector<string> classifications)
	{
		unordered_set<string> featureValuesSet;

		for (auto feature : featureValues)
			featureValuesSet.insert(feature);

		double entropy = 0;

		for (auto feature : featureValuesSet)
		{
			vector<string> featureClassifications;
			int occurences = 0;
			for (int i = 0; i < featureValues.size(); i++)
			{
				if (featureValues[i] == feature)
				{
					occurences++;
					featureClassifications.push_back(classifications[i]);
				}

				double classificationsEntropy = calculateClassificationsEntropy(featureClassifications);

				entropy += ((double)occurences / (double)featureValues.size()) * classificationsEntropy;
			}
		}

		return entropy;
	}

	// Помощна функция, която намира "ентропията" на класовете за тези данни, които има конкретна стойност на даден feature
	double calculateClassificationsEntropy(vector<string> classifications)
	{
		unordered_set<string> classesSet;

		for (auto feature : classifications)
			classesSet.insert(feature);

		if (classesSet.size() == 0)
			return 0;

		double entropy = 0;
		for (auto clazz : classesSet)
		{
			int occurences = 0;
			for (auto c : classifications)
				if (c == clazz)
					occurences++;
			double prob = (double)occurences / (double)classifications.size();
			entropy += prob * log2(prob);
		}

		return entropy;
	}


	// Помощна функция, която връща всички стойности на даден feature(колонка в таблицата от данни)
	vector<string> getFeatureValues(vector<unordered_map<int, string> > data, int feature)
	{
		vector<string> result;
		for (auto x : data)
			result.push_back(x[feature]);

		return result;
	}

	// Помощна функция, която връща уникалните стойности на даден feature(уникалните елементи от колонка в таблицата от данни)
	unordered_set<string> getFeatureValuesSet(vector<unordered_map<int, string> > data, int feature)
	{
		unordered_set<string> result;
		for (auto x : data)
			result.insert(x[feature]);

		return result;
	}



public:
	DecisionTreeClassifier(int k) : k(k) {}
	// Фукнция, която класифицира един конкретен екземпляр.
	// Екземплярът е представен като Map от индекси на feature-ите му и съответно стойноститe за тези feature-и.
	// Пример {1: "10-19", 2: "lt40"}, където 1 е индексът на "age", 2 e "menopause"
	string predict(unordered_map<int, string> x)
	{
		Node* current = tree;
		// Обхождаме дървото, докато не стигнем листо
		while (current->clazz.empty())
		{
			// Вземаме стойността на екземпляра за текущия feature
			string featureValue = x[current->feature];

			// Намираме върха наследник, по който трябва да продължим
			if (current->children.find(featureValue) != current->children.end())
			{
				current = current->children[featureValue];
			}
			else
			{
				// Ако няма такъв, просто връщаме произволен клас - този сценарий би се случил само ако в екземпляра има непозната стойност на feature.
				// Друга алтернатива е да продължим към произволен наследник
				return *(classes.begin());
			}
		}
		// Когато стигнем листо, връщаме неговият клас като резултатен
		return current->clazz;
	}

	// Функция, която построява дървото за вземане на решение на база на данните.
	// `data` е списъкът с данни/екземпляри. Всеки екземплярът е представен като Map от индекси на feature-ите му и съответно стойноститe за тези feature-и.
	// Пример {1: "10-19", 2: "lt40"}, където 1 е индексът на "age", 2 e "menopause".
	// `classifications` е списък с класа на всеки екземпляр. `classifications[i]` e класът на `data[i]`.
	// Пример ["no-recurrence-events", "recurrence-events"]
	void train(vector<unordered_map<int, string> > data, vector<string> classifications)
	{

		for (string clazz : classifications)
			classes.insert(clazz);

		for (auto p : data[0])
			features.push_back(p.first);

		tree = buildTree(data, classifications, features);
	}

	~DecisionTreeClassifier() {
		delete this->tree;
	}
};

// Помощна функция, която прочита данните от csv файл.
// Данните във файла трябва да са във формат:  no-recurrence-events,30-39,premeno,30-34,0-2,no,3,left,left_low,no
// На първо място е класът, а после са всички feature-и
pair<vector<unordered_map<int, string> >, vector<string> > readData(string filename)
{

	ifstream fileStream(filename);
	vector<unordered_map<int, string> > data;
	vector<string> classifications;

	string line;
	while (getline(fileStream, line))
	{
		stringstream lineStream(line);
		string cell;
		int index = 0;
		unordered_map<int, string> x;

		while (getline(lineStream, cell, ','))
		{
			if (index == 0)
				classifications.push_back(cell);
			else
				x[index] = cell;

			index++;
		}

		data.push_back(x);
	}


	//Разбъркваме данните, тъй като във входния файл те са подредени по класове
	vector<int> indexes;
	indexes.reserve(data.size());
	for (int i = 0; i < data.size(); ++i) {
		indexes.push_back(i);
	}
	random_device rd;
	mt19937 g(rd());
	shuffle(data.begin(), data.end(), g);
	shuffle(classifications.begin(), classifications.end(), g);


	return make_pair(data, classifications);
}


//За 10 итерации се тренират 9/10 от данните и се тества с останалите 1/10. Тестовите данни се взимат последователно.
void crossvalidation(DecisionTreeClassifier& dtc, pair<vector<unordered_map<int, string> >, vector<string>>& input) {
	int testSize = input.first.size() / 10;
	int testStartIndex;
	double sumAccuracy = 0;

	for (int j = 0; j < 10; j++)

	{
		vector<unordered_map<int, string>>trainData;
		vector<string> trainClassification;

		vector<unordered_map<int, string>>testData;
		vector<string> testClassification;
		testStartIndex = j * testSize;

		for (int i = 0; i < input.first.size(); i++)
		{

			if (i >= testStartIndex && i < testStartIndex + testSize)
			{
				testData.push_back(input.first[i]);
				testClassification.push_back(input.second[i]);
			}
			else
			{
				trainData.push_back(input.first[i]);
				trainClassification.push_back(input.second[i]);
			}
		}


		dtc.train(trainData, trainClassification);
		double hits = 0;
		for (int i = 0; i < testSize; i++)
		{
			if (dtc.predict(testData[i]) == testClassification[i])
				hits += 1;
			
		}
		cout << "Test " << j + 1 << " accuracy: " << fixed << setprecision(1) << ((hits * 100) / testSize) << "%" << endl;
		sumAccuracy += ((hits * 100) / testSize);

	}

	cout << "---------------------"<<endl  << "Average accuracy is: " << sumAccuracy / 10 <<"%"<<endl;
}

int main()
{

	//Създаваме класификатора
	DecisionTreeClassifier dtc(1);

	auto input = readData("breast-cancer.data"); 
	
	//10-fold cross-validation
	crossvalidation(dtc,input);
	
	return 0;
}
