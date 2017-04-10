#pragma once
#include <vector>
#include <utility>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <string>
#include <set>

#include <algorithm>

const int SUCCESS = 0;
const int FAILURE = 1;

const int LIST_BEGIN_FLAG = -1;

/*
struct InvertedListItem
{
	int string_id;
	char* str;
	InvertedListItem* next;

	InvertedListItem()
	{
		string_id = LIST_BEGIN_FLAG;
	}
	InvertedListItem(int _string_id,char* _str):
		string_id(_string_id),str(_str),next(NULL)
	{

	}
	InvertedListItem(int _string_id,char* _str,InvertedListItem* _next):
		string_id(_string_id),str(_str),next(_next)
	{

	}
};
*/

class SimSearcher
{
public:
	SimSearcher();
	~SimSearcher();

	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);

	int createEntry(char* item, int id);
	int generateQgrams(const char* word, int word_length, std::vector<char*>& qgrams);
	int insertInvertedList(char* qgram, int id);
	int createED(char *item, int id);
	int createJCD(char *item, int id);

	int calED(const char *query, const char* entry, int th);

	int q;
	void printDebug(std::vector<std::pair<unsigned, unsigned> > &result);
};

