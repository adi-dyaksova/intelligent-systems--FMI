#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Помощна функция, която генерира число между from и to включително
int randomNumber(int from, int to)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(from, to);

    return dist(rng);
}

// Структура, която представя точка в евклидовото пространство
struct Point
{
    double x; // x координатата на точката
    double y; // y координатата на точката

    Point() : x(0), y(0) {}
    Point(double x, double y) : x(x), y(y) {}

    // Функция, която намира евклидовото разстояние между текущата точка и произволна друга
    double distanceTo(Point &other)
    {
        return std::sqrt(std::pow((other.x - x), 2) + std::pow((other.y - y), 2));
    }
};

class KMeans
{
private:
    int k;                              // Броят клъстъри, на които ще опитваме да групираме точките
    std::string initializationStrategy; // Стратегията, по която ще избираме центровете в началото
    std::vector<Point> clusterCenters;  // Центровете на k на брой клъстърите, които ще търсим
    int maxIterations;                  // максимален брой итерации, за които да търсим центровете на клъстърите

    // Функция, която връща индексa на клъстъра, към който принадлежи една точка
    int findClosestClusterCenter(Point &x)
    {
        int closestClusterIndex = 0;
        double minDistance = x.distanceTo(clusterCenters[0]);

        // Обхождаме клъстърните центрове и намираме индекса на този който е най-близко до точката x
        for (int i = 1; i < k; i++)
        {
            double currentDistance = x.distanceTo(clusterCenters[i]);
            if (currentDistance < minDistance) {
                closestClusterIndex = i;
                minDistance = currentDistance;
            }
                
        }

        return closestClusterIndex;
    }

    // Функция, която избира началните точки центрове
    void initClusterCenters(std::vector<Point> points)
    {
        if (initializationStrategy == "kmeans++")
        {   
            // Започваме с една произволна точка за център
            std::vector<Point> centers;
            int index = randomNumber(0, points.size() - 1);
            centers.push_back(points[index]);
            for (int i = 1; i < k; i++)
            {
                // На всяка стъпка намираме най-малкото разстояние от всяка точка до някой от текущите центрове.
                std::vector<double> distancesSquared(points.size());
                std::vector<double> probabilities(points.size());
                double sum = 0;
                for (int j = 0; j < points.size(); j++)
                {
                    double distance = minDistance(points[j], centers);
                    distancesSquared[j] = std::pow(distance, 2);
                    sum += distancesSquared[j];
                }
                for (int j = 0; j < distancesSquared.size(); j++)
                {
                    probabilities[j] = distancesSquared[j] / sum;
                }

                // Избираме с по-висока вероятност точките, които са по-далеч от текущите центрове.
                // Логично е клъстърите да са раздалечени един от друг.
                int newClusterCenterIndex = randomIndexWithProbability(probabilities);
                centers.push_back(points[newClusterCenterIndex]);
            }
            clusterCenters = centers;
        }
        else
        {
            // При random стратегията просто избираме произволни k точки за центрове
            for (int i = 0; i < k; i++)
            {
                int index = randomNumber(0, points.size() - i - 1);
                clusterCenters[i] = points[index];
                std::swap(points[index], points[points.size() - i - 1]);
            }
        }
    }

    // Функция, която преизчислява центровете
    void reevaluateClusterCenters(std::vector<Point> &points, std::vector<int> &results)
    {
        for (int cluster = 0; cluster < k; cluster++)
        {   
            // Всеки център на клъстър става средно аритметичното на всички точки в този клъстър
            std::vector<Point> pointsInCluster;
            for (int i = 0; i < results.size(); i++)
            {
                if (results[i] == cluster)
                {
                    pointsInCluster.push_back(points[i]);
                }
            }
            clusterCenters[cluster] = average(pointsInCluster);
        }
    }

    // Помощна функция, която намира средно аритметичната точка на списък от точки
    Point average(std::vector<Point> &points)
    {
        double totalX = 0;
        double totalY = 0;

        for (Point p : points)
        {
            totalX += p.x;
            totalY += p.y;
        }

        return Point(totalX / points.size(), totalY / points.size());
    }

    // Функция, която връща най-малкото разстояние от point до коя да е от точките в centers
    double minDistance(Point &point, std::vector<Point> &centers)
    {
        double min = point.distanceTo(centers[0]);

        for (Point c : centers)
        {
            double current = point.distanceTo(c);
            if (current < min)
                min = current;
        }

        return min;
    }

    // Функция, която връща произволен индекс на база на вероятността да се падне.
    // Пример: probabilities = [0.1, 0.4, 0.5] - това означава, че има 10% шанс да се падне индекс 0, 40% шанс да се падне индекс 1 и 50% за индекс 2.
    // За да симулираме това поведение може да генерираме число между 0 и 10.
    // Ако се падне 0, избираме индекс 0. Ако се падне нещо между 1 и 4, избираме индекс 1. Ако се падне нещо между 5 и 10, избираме индекс 2.
    // За да сме по-прецизни, ще генерираме число между 0 и 100000, но логиката остава същата.
    int randomIndexWithProbability(std::vector<double> &probabilities)
    {
        int random = randomNumber(0, 100000);

        double sum = 0;
        
        for (int i = 0; i < probabilities.size(); i++)
        {
            double newSum = sum + 100000 * probabilities[i];
            if (random >= sum && random < newSum)
            {
                return i;
            }
            sum = newSum;
        }

        return probabilities.size() - 1;
    }

public:
    KMeans(int k, std::string initializationStrategy) : k(k), clusterCenters(k), initializationStrategy(initializationStrategy), maxIterations(100000) {}

    // Функция, която тренира алгоритъма и намира клъстерните цетнрове на списък от точки.
    void train(std::vector<Point> &points)
    {   
        // Инициализираме центровете
        initClusterCenters(points);
        
        // Започваме да клъстеризираме данните
        std::vector<int> results(points.size(), -1);
        bool clusteringHasChanged = true;
        int iteration = 0;
        // Докато има промяна клъстеризирането или докато стигнем максималния брой итерации
        while (clusteringHasChanged && iteration < maxIterations)
        {
            clusteringHasChanged = false;
            iteration++;
            // За всяка точка проверяваме кой е новият най-близък център до нея
            for (int i = 0; i < points.size(); i++)
            {
                int cluster = findClosestClusterCenter(points[i]);
                if (results[i] != cluster)
                {
                    clusteringHasChanged = true;
                    results[i] = cluster;
                }
            }
            if (clusteringHasChanged)
            {
                // Ако има промяна, трябва да преизчислим центровете
                reevaluateClusterCenters(points, results);
            }
        }
    }

    // Функция, която връща индексa на клъстъра, към който принадлежи една точка
    int predict(Point x)
    {
        return findClosestClusterCenter(x);
    }

    // Помощна функция, която връща клъстерния център на индекс i
    Point &getCenter(int i)
    {
        return clusterCenters[i];
    }
};

// Помощна функция, която прочита данните от файл.
// Данните във файла трябва да са във формат:
//
// 1 2
// 5.3 2.3
// -2 7
//
// На първо място е x координатата на точката, а на второ е y координатата
std::vector<Point> readData(std::string filename)
{
    std::ifstream fileStream(filename);

    std::vector<Point> result;

    std::string line;
    while (std::getline(fileStream, line))
    {
        std::stringstream lineStream(line);
        double x, y;
        lineStream >> x >> y;
        result.push_back(Point(x, y));
    }

    return result;
}

// Помощна функция, която записва резултатите във файл.
// На първия ред във файла ще е вътрешното клъстерно разстояние и след това на всеки ред ще има по една точка и цвета на клъстъра й:
//
// 34.4209
// 5.275 4.893 blue
// 5.339 4.476 red
//
void outputResults(
    std::string filename,
    double wccs,
    std::vector<Point> &data,
    std::vector<int> &results,
    std::unordered_map<int, std::string> &colors)
{
    std::ofstream fileStream(filename);
    fileStream << wccs << std::endl;
    for (int i = 0; i < results.size(); i++)
    {
        fileStream << data[i].x << " " << data[i].y << " " << colors[results[i]] << std::endl;
    }
}

int main()
{
    // Името на файла с входните данни - normal/unbalance
    std::string inputFile = "unbalance";
    // Броят клъстери, на който искаме да групираме входните данни
    int clusters = 8;
    // Стратегия за избиране на началните центрове - random/kmeans++
    std::string initializationStrategy = "kmeans++";
    // Цветът за всеки клъстър. Елементите на този map трябва да са поне колкото на брой клъстъра ще търсим.
    std::unordered_map<int, std::string> colors;
    colors[0] = "blue";
    colors[1] = "red";
    colors[2] = "green";
    colors[3] = "yellow";
    colors[4] = "black";
    colors[5] = "orange";
    colors[6] = "brown";
    colors[7] = "grey";
    colors[8] = "pink";
    colors[9] = "purple";

    // Прочитаме входните данни
    std::vector<Point> data = readData(inputFile + ".txt");

    // Тренираме алгоритъма върху тях
    KMeans kMeans(clusters, initializationStrategy);
    kMeans.train(data);

    std::vector<int> results;
    double wccs = 0; //  within-cluster sum of squares (WCSS)

    // Обхождаме точките и виждаме коя към кой център принадлежи
    for (Point p : data)
    {
        int center = kMeans.predict(p);
        results.push_back(center);
        wccs += std::pow(p.distanceTo(kMeans.getCenter(center)), 2);
    }

    // Записваме резултатите във файл
    outputResults(
        inputFile + "-" + initializationStrategy + "-" + std::to_string(clusters) + ".txt",
        wccs,
        data,
        results,
        colors);

    return 0;
}