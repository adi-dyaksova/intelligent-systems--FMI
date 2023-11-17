
#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <functional>
#include <iostream>
#include <cmath>
using namespace std;

int M, N;
const int MAX_CNT_POP = 100;
double MUTATION_PERCENTAGE = 0.05;

struct Item{
	int weight;
	int value;
};
struct Knapsack{
	int fitness = 0;
	vector<int> items;
};

bool compareKnapsacks(Knapsack& k1, Knapsack& k2) {
	return (k1.fitness > k2.fitness);
}

vector<Knapsack> population(MAX_CNT_POP); 
vector<Knapsack> nextPopulation(MAX_CNT_POP);

int calc_fitness(vector<int>& curr_knapsack_items, vector<Item>& items) {
	int fitness = 0;
	int weight_sum = 0;
	for (int i = 0; i < curr_knapsack_items.size(); i++)
	{
		if (curr_knapsack_items[i] == 1) {
			fitness += items[i].value;
			weight_sum += items[i].weight;
		}
		if (weight_sum > M) {
			fitness = 0; //return 0 if max weight is exceeded
			break;
		}
	}
	return fitness;
}

void generatePopulation(vector<Item>& items) {
	vector<int> curr_items(N);
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, 1);

	for (int i = 0; i < MAX_CNT_POP; i++)
	{
		for (int j = 0; j < curr_items.size(); j++)
		{
			curr_items[j] = distrib(gen);
		}

		Knapsack k;
		k.items = curr_items;
		k.fitness = calc_fitness(k.items, items);
		if (k.fitness == 0) {
			i--;
		}
		else{
			population[i] = k; 
		}
	}
	sort(population.begin(), population.end(), compareKnapsacks); 
}

void crossover(vector<Item>& items) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, N - 1);
	int point = distrib(gen);

	vector<Knapsack> nextPopulation;

	//elitism - preserve best parent in next population
	Knapsack best;
	best.fitness = population[0].fitness;
	best.items = vector<int>(population[0].items);
	nextPopulation.push_back(best);

	//random selection of parents
	vector<int> indexes(population.size());
	for (int i = 0; i < population.size(); i++) {
		indexes[i] = i;
	}
	random_shuffle(indexes.begin(), indexes.end());
	
	for (int i = 0; i < population.size() - 1; i += 2)
	{
		Knapsack child1;
		child1.fitness = 0;
		Knapsack child2;
		child2.fitness = 0;

		while (child1.fitness == 0 || child2.fitness == 0) {
			point = distrib(gen);
			child1.items.clear();
			child2.items.clear();
			for (int j = 0; j < N; j++) {
				if (j <= point) {
					child1.items.push_back(population[indexes[i]].items[j]);
					child2.items.push_back(population[indexes[i + 1]].items[j]);
				}
				else {
					child1.items.push_back(population[indexes[i + 1]].items[j]);
					child2.items.push_back(population[indexes[i]].items[j]);
				}
			}
			child1.fitness = calc_fitness(child1.items, items);
			child2.fitness = calc_fitness(child2.items, items);
		}
		nextPopulation.push_back(child1);
		nextPopulation.push_back(child2);
	}

	population = nextPopulation;
	sort(population.begin(), population.end(), compareKnapsacks);
	population.pop_back();
}

void mutate(vector<Item>& items) { 
	int mutation_cnt = ceil(N * MUTATION_PERCENTAGE);
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib_gene(0, N - 1);
	uniform_int_distribution<> distrib_pop(1, population.size() - 1); //from 1 in order not to change the best knapsack


	for (int i = 0; i < mutation_cnt; i++)
	{		
		int gene = distrib_gene(gen);
		int pop_ind = distrib_pop(gen);

		if (population[pop_ind].items[gene] == 0) {
			population[pop_ind].items[gene] = 1;
			int newFitness = calc_fitness(population[pop_ind].items, items);
			
			if (newFitness == 0) {
				population[pop_ind].items[gene] = 0;
			}
			else {
				population[pop_ind].fitness = newFitness;
			}
		}
		else {
			population[pop_ind].items[gene] = 0;
			population[pop_ind].fitness -= items[gene].value;
		}
	}
	sort(population.begin(), population.end(), compareKnapsacks);
}

int main() {
	cin >> M >> N;
	vector<Item> items(N);
	vector<int> indexes(N);

	for (int i = 0; i < N; i++)
	{
		cin >> items[i].weight >> items[i].value;
		indexes[i] = i;
	}

	generatePopulation(items);
	sort(population.begin(), population.end(), compareKnapsacks);

	int best = 0;
	for (int i = 0; i <= 10000; i++) 
	{
		crossover(items);
		mutate(items);
		
		if (i == 0 || i == 10 || i == 200 || i == 5000 || i == 10000) {

			cout << "Generation " << i << ": " << endl;
			best = 0;
			for (int j  = 0; j  < population.size(); j ++)
			{
				if (best < population[j].fitness) {
					best = population[j].fitness;
				}
			}
			cout << "best fitness value: " << best << endl;
		}
	}

	return 0;
}