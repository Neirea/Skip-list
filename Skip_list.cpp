#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <set>

using namespace std;

// random int number generator
int genRandInt(int min, int max)
{
	static default_random_engine ran;
	return uniform_int_distribution<>{min, max}(ran);
}
/*rng generated number of levels for inserted node;
flipping coin [0 or 1] so function returns number of
levels to fill out(not more than maxLevels)*/
int numberOfLevels(int current, int max)
{
	int i = 1;
	//added check for currentLevelMax so it doesn't go more than 1 ahead of it and can't go above maxLevel
	while (genRandInt(0, 1) != 0 && i <= current && i < max - 1)
		++i;
	return i;
}

template <typename T>
class Skip_list
{

public:
	struct Node
	{
		T value;
		vector<Node *> next;

		Node(T k, int level)
			: value(k)
		{
			for (int i = 0; i < level; ++i)
				next.push_back(nullptr);
		}
	};
	Skip_list()
	{
		//creating head pointer with min T value and maxlevel(16) of succesors
		constexpr auto headValue = numeric_limits<T>::min();
		head = new Node(headValue, maxLevel);

		//creating tail pointer with max T value and maxlevel(16) of succesors
		constexpr auto tailValue = numeric_limits<T>::max();
		tail = new Node(tailValue, maxLevel);

		//making connection from head to tail
		for (int i = 0; i < maxLevel; ++i)
		{
			head->next[i] = tail;
		}
	}
	~Skip_list() noexcept
	{
		auto node = head;
		while (node->next[0] != nullptr)
		{
			auto tmp = node;
			node = node->next[0];
			delete tmp;
		}
		delete node;
	}

	void insert(T searchValue)
	{
		Node *currentNode = head;

		//rng roll if we go for 1 NodeLevel higher
		int newNodeLevel = numberOfLevels(currentLevelMax, maxLevel);
		//creating new node and checking if new node will have more levels than currentLevelMax
		auto insertedNode = new Node(searchValue, newNodeLevel);
		if (currentLevelMax < newNodeLevel)
			currentLevelMax = newNodeLevel;

		//stores nodes that will be updated
		vector<Node *> update(newNodeLevel);

		//searching for closest by value node
		for (int i = newNodeLevel - 1; i >= 0; --i)
		{
			while (currentNode->next[i]->value < searchValue)
			{
				currentNode = currentNode->next[i];
			}
			update[i] = currentNode;
		}
		//inserting node and setting its connections up to current max level
		for (int i = 0; i < newNodeLevel; ++i)
		{
			insertedNode->next[i] = update[i]->next[i];
			update[i]->next[i] = insertedNode;
		}
	}

	Node *find(T searchValue) const
	{
		Skip_list::Node *currentNode = head;

		//searching for closest by value value node
		for (int i = currentLevelMax - 1; i >= 0; --i)
		{
			while (currentNode->next[i] != nullptr && currentNode->next[i]->value < searchValue)
				currentNode = currentNode->next[i];
		}
		//and moving to next node which should be a result of finding
		currentNode = currentNode->next[0];

		//checks if it's actually is
		if (currentNode->value == searchValue)
			return currentNode;
		//otherwise return tail pointer
		return tail;
	}

	void erase(T searchValue)
	{
		Node *currentNode = head;
		//stores nodes that will be updated
		vector<Node *> update(currentLevelMax);

		//searching for closest by value node
		for (int i = currentLevelMax - 1; i >= 0; --i)
		{
			while (currentNode->next[i]->value < searchValue)
			{
				currentNode = currentNode->next[i];
			}
			update[i] = currentNode;
		}
		//and moving to next node which should be a result of finding
		currentNode = currentNode->next[0];

		//check if if node exists
		if (currentNode->value != searchValue)
			return;
		for (int i = 0; i < currentNode->next.size(); ++i)
		{
			update[i]->next[i] = currentNode->next[i];
		}
		delete currentNode;
	}

	void print() const
	{
		Node *currentNode = head;

		while (currentNode->next[0] != nullptr && currentNode->next[0] != tail)
		{
			cout << "value: " << currentNode->next[0]->value
				 << " , levels: " << currentNode->next[0]->next.size();

			currentNode = currentNode->next[0];
			cout << endl
				 << endl;
		}
	}

private:
	Node *head; // value equals min of T
	Node *tail; // value equals max of T

	// max number of extra levels
	const int maxLevel = 16;
	//  helper variable to get current max number of nodes levels
	int currentLevelMax = 1;
};

int main()
{
	// Simple test:
	int maxNumber = 99999;
	Skip_list<int> list1;
	for (int i = 0; i < maxNumber; ++i)
	{
		list1.insert(i);
	}

	//Testing time result of find and erase function for Skip List
	auto start = chrono::high_resolution_clock::now();

	int searchValue = maxNumber - 1;
	for (int i = 0; i < maxNumber / 2; ++i)
	{
		list1.find(searchValue);
		searchValue -= 2;
	}
	searchValue = maxNumber - 1;
	for (int i = 0; i < maxNumber / 2; ++i)
	{
		list1.erase(searchValue);
		searchValue -= 2;
	}

	auto end = chrono::high_resolution_clock::now();
	double time_taken = (1e-9) * chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	cout << "Time taken by Skip List is : " << fixed
		 << time_taken << setprecision(9);
	cout << " sec" << endl;

	//---------------------------------------------------------------------------

	set<int> list2;
	for (int i = 0; i < maxNumber; ++i)
	{
		list2.insert(i);
	}

	//Testing time result of find and erase function for set
	start = chrono::high_resolution_clock::now();

	searchValue = maxNumber - 1;
	for (int i = 0; i < maxNumber / 2; ++i)
	{
		list2.find(searchValue);
		searchValue -= 2;
	}
	searchValue = maxNumber - 1;
	for (int i = 0; i < maxNumber / 2; ++i)
	{
		list2.erase(searchValue);
		searchValue -= 2;
	}

	end = chrono::high_resolution_clock::now();
	time_taken = (1e-9) * chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	cout << "Time taken by Set is : " << fixed
		 << time_taken << setprecision(9);
	cout << " sec" << endl;
}
