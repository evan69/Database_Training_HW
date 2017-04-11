#include "SimSearcher.h"

using namespace std;

#define getMin(a,b,c) (min(min(a,b),c))
#define MAX_NUM 1000000
#define DIS_SIZE 4096

struct Entry
{
	char* word;
	int id;//word id
	int length;
};

struct Qgram
{
	char* qgram;
	int index;//inverted list index
};

string string_buf;
char* buffer;
int dis[4096][4096];
int candidate_count[MAX_NUM];

vector<Entry> word_vec;
vector<int> word_num;
vector<vector<int> > inverted_list;
vector<vector<int> > word_inverted_list;
set<Qgram> qgram_set;
set<Qgram> word_set;

int smin = MAX_NUM;

bool operator<(const Qgram& a,const Qgram& b)
{
	return strcmp(a.qgram, b.qgram) < 0;
}
bool operator==(const Qgram& a,const Qgram& b)
{
	return strcmp(a.qgram, b.qgram) == 0;
}

void scanCount(vector<vector<int> >& para_inverted_list,set<Qgram>& para_qgram_set,vector<char*>& query_qgrams,
	int th,vector<int>& candidate)
{
	candidate.clear();
	for(unsigned i = 0;i < word_vec.size() + 100;i++)
	{
		candidate_count[i] = 0;
	}
    Qgram q;
	for(vector<char*>::iterator it = query_qgrams.begin();it != query_qgrams.end();++it)
	{
		q.qgram = *it;
		set<Qgram>::iterator j = para_qgram_set.find(q);
		if(j == para_qgram_set.end())
			continue;
		vector<int>& inverted_list_entry = para_inverted_list[j->index];
		for(vector<int>::iterator k = inverted_list_entry.begin();k != inverted_list_entry.end();++k)
		{
			candidate_count[*k]++;
			if(candidate_count[*k] == th)
			{
				candidate.push_back(*k);
			}
		}
	}
}

SimSearcher::SimSearcher()
{
}

SimSearcher::~SimSearcher()
{
}

int SimSearcher::createEntry(char* item, int id)
{
	assert(id == (int)word_vec.size());
	assert(id == (int)word_num.size());
	char* new_word = new char[4096];
	strcpy(new_word,item);
	Entry e;
	e.word = new_word;
	e.id = id;
	e.length = (int)strlen(new_word);
	word_vec.push_back(e);

	vector<char*> qgrams;
	//cout << "generateQgrams:" << item << endl;
	generateQgrams(item,e.length,qgrams);
	for(vector<char*>::iterator it = qgrams.begin();it != qgrams.end();++it)
	{
		insertInvertedList(*it,id);
	}
	//cout << "end generateQgrams" << endl;

	vector<string> words;
	splitWord(item,' ',words);
	word_num.push_back((int)words.size());
	smin = smin > (int)words.size() ? (int)words.size() : smin;
	for(vector<string>::iterator it = words.begin();it != words.end();++it)
	{
		char* add = new char[it->length() + 10];
		for(unsigned j = 0;j < it->length();j++)
		{
			add[j] = (*it)[j];
		}
		add[it->length()] = '\0';
		insertWordInvertedList(add,id);//for Jac
	}
	return SUCCESS;
}

int SimSearcher::generateQgrams(const char* word, int word_length, vector<char*>& qgrams)
{
	qgrams.clear();
	int i = 0;
	int len = word_length;
	while(i + q - 1 < len)
	{
		char* qgram_entry = new char[this->q + 1];
		for(int j = 0;j < q;j++)
		{
			qgram_entry[j] = word[j + i];
		}
		qgram_entry[q] = '\0';
		qgrams.push_back(qgram_entry);
		i++;
	}
	return SUCCESS;
}

int SimSearcher::splitWord(const char* line, char c, vector<string>& words)
{
	string str = string(line);
	int len = str.length();
    int start = 0;
    for(int i = 0;i<len;i++)
    {
        if(str[i] == c)
        {
            words.push_back(str.substr(start,i-start));
            while(str[i+1] == c)
            {
                i++;
            }
            start = i+1;
        }
    }
    words.push_back(str.substr(start,len-start));
    sort(words.begin(),words.end());
    words.erase(unique(words.begin(),words.end()),words.end());
    return SUCCESS;
}

int SimSearcher::insertInvertedList(char* qgram, int id)
// build inverted list. id begin at the second item
{
	int new_index = inverted_list.size();
	Qgram new_qgram;
	new_qgram.qgram = qgram;
	new_qgram.index = new_index;
	set<Qgram>::iterator it = qgram_set.find(new_qgram);
	if(it == qgram_set.end())
	{
		inverted_list.push_back(vector<int>());
		qgram_set.insert(new_qgram);
		insertInvertedList(qgram,id);
	}
	else
	{
		int index = it->index;
		inverted_list[index].push_back(id);
	}
	return SUCCESS;
}

int SimSearcher::insertWordInvertedList(char* item, int id)
{
	int new_index = word_inverted_list.size();
	Qgram new_word;
	new_word.qgram = item;
	new_word.index = new_index;
	set<Qgram>::iterator it = word_set.find(new_word);
	if(it == word_set.end())
	{
		word_inverted_list.push_back(vector<int>());
		word_set.insert(new_word);
		insertWordInvertedList(item,id);
	}
	else
	{
		int index = it->index;
		word_inverted_list[index].push_back(id);
	}
	return SUCCESS;
}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
	this->q = q;
	ifstream fin(filename);
	int id = 0;
	
	while(!fin.eof())
	{
		getline(fin,string_buf);
		if(string_buf.size() == 0)
		{
			break;
		}
		char* buffer = &string_buf[0];
		createEntry(buffer,id);
		id++;
	}
	
	fin.close();

	for (int i = 0; i < DIS_SIZE; i++)
		dis[i][0] = i;
	for (int j = 0; j < DIS_SIZE; j++)
		dis[0][j] = j;

	return SUCCESS;
}

int SimSearcher::calED(const char *query, const char* entry, int th)
{
	//cout << "calED" << "\n";
	const char* t = query;
	int lent = strlen(t);
	const char* s = entry;
	int lens = strlen(s);
	if(abs(lens - lent) > th)
		return MAX_NUM;
	if(lens > lent)
		return calED(entry, query, th);
	
	for (int i = 1; i <= lens; i++)
	{
		int lo = max(1,i-th);
		int hi = min(lent,i+th);
		bool flag = true;
		for (int j = lo; j <= hi; j++)
		{
			int tij = (s[i - 1] == t[j - 1]) ? 0 : 1;
			
			if(j == i-th)
			{
				dis[i][j] = min(dis[i - 1][j] + 1,
	            			   dis[i - 1][j - 1] + tij);
			}
			else if(j == i+th)
			{
				dis[i][j] = min(dis[i][j - 1] + 1,
	            			   dis[i - 1][j - 1] + tij);
			}
			else
			{
				dis[i][j] = getMin(dis[i - 1][j] + 1,
	            			   dis[i][j - 1] + 1,
	            			   dis[i - 1][j - 1] + tij);
			}
			if(dis[i][j] <= th)
				flag = false;
		}
		if(flag)
			return MAX_NUM;
	}
	return dis[lens][lent];
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
    vector<string> str_words;
    splitWord(query,' ',str_words);

    vector<char*> words;
    for(vector<string>::iterator it = str_words.begin();it != str_words.end();it++)
    {
		char* tmp = new char[it->length() + 10];
		for(unsigned j = 0;j < it->length();j++)
		{
			tmp[j] = (*it)[j];
		}
		tmp[it->length()] = '\0';
		words.push_back(tmp);
    }

    vector<int> candidate;
    int word_size = words.size();
    int T = max(threshold*word_size,(smin+word_size)*threshold/(threshold+1));
    scanCount(word_inverted_list,word_set,words,T,candidate);
    sort(candidate.begin(),candidate.end());
    if(T > 0)
    {
        unsigned size = candidate.size();
        for(unsigned i = 0;i < size;i++)
        {
			int id = candidate[i];
			double jaccard = double(candidate_count[id])/double(word_num
				[id]+word_size-candidate_count[id]);
			if(jaccard>=threshold)
				result.push_back(pair<unsigned,double>(id,jaccard));
        }
    }
    else
    {
        for(unsigned i = 0;i < word_vec.size();i++)
        {
			double jaccard = double(candidate_count[i])/double(word_num
				[i]+word_size-candidate_count[i]);
			if(jaccard>=threshold)
				result.push_back(pair<unsigned,double>(i,jaccard));
        }
    }

	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	vector<char*> query_qgrams;
	int len = strlen(query);
	int th = len - q + 1 - q * threshold;

	//if(th > 0)
	if(false)
	{
		generateQgrams(query,len,query_qgrams);
		for(unsigned i = 0;i < word_vec.size() + 100;i++)
		{
			candidate_count[i] = 0;
		}

		vector<int> candidate;
		Qgram q;
		for(vector<char*>::iterator it = query_qgrams.begin();it != query_qgrams.end();++it)
		{
			q.qgram = *it;
			set<Qgram>::iterator j = qgram_set.find(q);
			if(j == qgram_set.end())
				continue;
			vector<int>& inverted_list_entry = inverted_list[j->index];
			for(vector<int>::iterator k = inverted_list_entry.begin();k != inverted_list_entry.end();++k)
			{
				candidate_count[*k]++;
				if(candidate_count[*k] == th)
				{
					candidate.push_back(*k);
				}
				
			}
		}

		sort(candidate.begin(),candidate.end());
		for(vector<int>::iterator it = candidate.begin();it != candidate.end();++it)
		{
			int j = *it;
			char* candidate_word = (word_vec[j]).word;
			int ed = calED(query,candidate_word,threshold);
			if(ed <= (int)threshold)
			{
				result.push_back(make_pair(*it,ed));
			}
		}
	}
	else
	{
		for(vector<Entry>::iterator it = word_vec.begin();it != word_vec.end();++it)
		{
			char* candidate_word = it->word;
			if(abs(it->length - len) > threshold)
				continue;
			int ed = calED(query,candidate_word,threshold);
			//cout << candidate_word << " " << ed << endl;
			if(ed <= (int)threshold)
			{
				result.push_back(make_pair(it->id,ed));
			}
		}
	}

/*
	for(vector<Entry>::iterator it = entry_vec.begin();it != entry_vec.end();++it)
	{
		int ed = calED(query,it->word);
		if(ed <= (int)threshold)
		{
			result.push_back(make_pair(it->id,ed));
		}
	}
*/	return SUCCESS;
}



void SimSearcher::printDebug(vector<pair<unsigned, unsigned> > &result)
{
	cout << "ED------------------------\n";

	for(set<Qgram>::iterator it = qgram_set.begin();it != qgram_set.end();++it)
	{
		int index = it->index;
		vector<int>& list = inverted_list[index];
		cout << it->qgram << " : ";
		for(vector<int>::iterator it2 = list.begin();it2 != list.end();++it2)
		{
			cout << *it2 << " , ";
		}
		cout << endl;
	}
	cout << "result:" << endl;
	for(vector<pair<unsigned, unsigned> >::iterator it = result.begin();it != result.end();++it)
	{
		cout << it->first << ":" << word_vec[it->first].word << " ed: " << it->second << endl;
	}

	cout << "JAC-----------------------\n";

	for(set<Qgram>::iterator it = word_set.begin();it != word_set.end();++it)
	{
		int index = it->index;
		vector<int>& list = word_inverted_list[index];
		cout << it->qgram << " : ";
		for(vector<int>::iterator it2 = list.begin();it2 != list.end();++it2)
		{
			cout << *it2 << " , ";
		}
		cout << endl;
	}

	cout << "end-------------------------\n";
}
