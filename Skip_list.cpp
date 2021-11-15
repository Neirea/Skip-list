#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <set>
#define MAX_LEVEL 16

using namespace std;

// random int number generator
short genRandInt(short min, short max)
{
	static default_random_engine ran;
	return uniform_int_distribution<>{min, max}(ran);
}
/*rng generated number of levels for inserted node;
flipping coin [0 or 1] so function returns number of
levels to fill out(not more than MAX_LEVELs)*/
short numberOfLevels(short current)
{
	short i = 1;
	//added check for currentLevelMax so it doesn't go more than 1 ahead of it and can't go above MAX_LEVEL
	while (genRandInt(0, 1) != 0 && i <= current && i < MAX_LEVEL)
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
		Node **next;
		short size; // don't really need it but used for nice printing

		Node(T &k, short level)
			: value(k), size(level)
		{
			next = new Node *[level];
			//fills next with default values
			memset(next, NULL, sizeof(Node *) * (level));
		}
		~Node()
		{
			delete[] next;
		}
	};
	Skip_list()
	{
		//creating head pointer with min T value and MAX_LEVEL of succesors
		auto headValue = numeric_limits<T>::min();
		head = new Node(headValue, MAX_LEVEL);

		//creating tail pointer with max T value and MAX_LEVEL of succesors
		auto tailValue = numeric_limits<T>::max();
		tail = new Node(tailValue, MAX_LEVEL);

		//making connection from head to tail
		for (short i = 0; i < MAX_LEVEL; ++i)
		{
			head->next[i] = tail;
		}
	}
	~Skip_list() noexcept
	{
		auto currentNode = head->next[0];
		while (currentNode != tail)
		{
			auto tmp = currentNode->next[0];
			delete currentNode;
			currentNode = tmp;
		}
		delete head;
		delete tail;
	}

	void insert(T &searchValue)
	{
		Node *currentNode = head;

		//rng roll if we go for 1 NodeLevel higher
		short newNodeLevel = numberOfLevels(currentLevelMax);
		//creating new node and checking if new node will have more levels than currentLevelMax
		auto insertedNode = new Node(searchValue, newNodeLevel);
		if (currentLevelMax < newNodeLevel)
		{
			currentLevelMax = newNodeLevel;
		}

		//stores nodes that will be updated
		Node *update[MAX_LEVEL];
		memset(update, 0, sizeof(Node *) * (MAX_LEVEL));

		//searching for closest by value node
		for (short i = newNodeLevel - 1; i >= 0; --i)
		{
			while (currentNode->next[i]->value < searchValue)
			{
				currentNode = currentNode->next[i];
			}
			update[i] = currentNode;
		}
		//inserting node and setting its connections up to current max level
		for (short i = 0; i < newNodeLevel; ++i)
		{
			insertedNode->next[i] = update[i]->next[i];
			update[i]->next[i] = insertedNode;
		}
	}

	Node *find(T &searchValue) const
	{
		Node *currentNode = head;

		//searching for closest by value value node
		for (short i = currentLevelMax - 1; i >= 0; --i)
		{
			while (currentNode->next[i]->value < searchValue)
			{
				currentNode = currentNode->next[i];
			}
		}
		//and moving to next node which should be a result of finding
		currentNode = currentNode->next[0];

		//checks if it's false result which returns tail value
		if (currentNode->value != searchValue)
		{
			//cout << "Wasn't able to find " << searchValue << endl;
			return tail;
		}
		//otherwise return result of finding
		return currentNode;
	}

	void erase(T &searchValue)
	{
		Node *currentNode = head;
		//stores nodes that will be updated
		Node *update[MAX_LEVEL];
		memset(update, 0, sizeof(Node *) * (MAX_LEVEL));

		//searching for closest by value node
		for (short i = currentLevelMax - 1; i >= 0; --i)
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
		{
			//cout << "Wasn't able to find and erase " << searchValue << endl;
			return;
		}

		for (short i = 0; i < currentLevelMax; ++i)
		{
			if (update[i]->next[i] != currentNode)
				break;
			update[i]->next[i] = currentNode->next[i];
		}
		delete currentNode;
		//check if we deleted node was highest level node
		if (currentLevelMax > 0 && head->next[currentLevelMax - 1] == tail)
			currentLevelMax--;
	}

	void print() const
	{
		Node *currentNode = head;

		while (currentNode->next[0] != nullptr && currentNode->next[0] != tail)
		{
			cout << "value: " << currentNode->next[0]->value
				 << " , levels: " << currentNode->size;

			currentNode = currentNode->next[0];
			cout << endl
				 << endl;
		}
	}

private:
	Node *head; // value equals min of T
	Node *tail; // value equals max of T

	//  helper variable to get current max number of nodes levels
	short currentLevelMax = 1;
};

// Simple test:
void test()
{

	int maxnumber = 99999;

	int searchvalue = 0;
	double time_taken = 0;
	auto start = chrono::high_resolution_clock::now();
	auto end = chrono::high_resolution_clock::now();

	Skip_list<int> list1;
	for (int i = 0; i < maxnumber; ++i)
	{
		list1.insert(i);
	}

	//testing time result of find and erase function for skip list
	start = chrono::high_resolution_clock::now();

	searchvalue = maxnumber - 1;
	for (int i = 0; i < maxnumber / 2; ++i)
	{
		list1.find(searchvalue);
		searchvalue -= 2;
	}

	searchvalue = maxnumber - 1;
	for (int i = 0; i < maxnumber / 2; ++i)
	{
		list1.erase(searchvalue);
		searchvalue -= 2;
	}

	end = chrono::high_resolution_clock::now();
	time_taken = (1e-9) * chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	cout << "time taken by skip list is : " << fixed
		 << time_taken << setprecision(9);
	cout << " sec" << endl;

	//---------------------------------------------------------------------------

	set<int> list2;
	for (int i = 0; i < maxnumber; ++i)
	{
		list2.insert(i);
	}

	//testing time result of find and erase function for set
	start = chrono::high_resolution_clock::now();

	searchvalue = maxnumber - 1;
	for (int i = 0; i < maxnumber / 2; ++i)
	{
		list2.find(searchvalue);
		searchvalue -= 2;
	}
	searchvalue = maxnumber - 1;
	for (int i = 0; i < maxnumber / 2; ++i)
	{
		list2.erase(searchvalue);
		searchvalue -= 2;
	}

	end = chrono::high_resolution_clock::now();
	time_taken = (1e-9) * chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	cout << "time taken by set is : " << fixed
		 << time_taken << setprecision(9);
	cout << " sec" << endl;
}

int main()
{
	test();
	return 0;
}