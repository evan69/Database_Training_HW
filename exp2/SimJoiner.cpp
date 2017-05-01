#include "SimJoiner.h"

using namespace std;

#define getMin(a,b,c) (min(min(a,b),c))
// min of 3 num
#define MAX_NUM 1000000
// a large number
#define DIS_SIZE 4096

vector<string> line_vec;
//vector of lines in input file
int dis[4096][4096];
//used in DP calculation
vector<vector<int> > gram_inverted_list;
//inverted list for ED
vector<vector<int> > word_inverted_list;
//inverted list for JAC
map<string,int> gram_set;
//set of gram for ED
map<string,int> word_set;
//set of word for JAC

int qq = 2;

SimJoiner::SimJoiner() {
	for (int i = 0; i < DIS_SIZE; i++)
		dis[i][0] = i;
	for (int j = 0; j < DIS_SIZE; j++)
		dis[0][j] = j;
}

SimJoiner::~SimJoiner() {
}

int split_gram(const string& str, int q, vector<string>& result, unsigned threshold)
{
	result.clear();
	int i = 0;
	int len = str.length();
	while(i + q - 1 < len)
	{
		string qgram_entry = string(q,'*');
		for(int j = 0;j < q;j++)
		{
			qgram_entry[j] = str[j + i];
		}
		//qgram_entry[q] = '\0';
		//cout << "entry:" << qgram_entry << endl;
 		result.push_back(qgram_entry);
		i++;
		if((unsigned)i > threshold * qq)
			break;
	}
	sort(result.begin(),result.end());
    result.erase(unique(result.begin(),result.end()),result.end());
	return SUCCESS;
}

int readin(const char *filename)
// read in data
{
	ifstream fin(filename);
	int id = 0;
	string string_buf;
	while(!fin.eof())
	{
		getline(fin,string_buf);
		if(string_buf.size() == 0)
		{
			break;
		}
		line_vec.push_back(string_buf);
		id++;
	}
	
	fin.close();
	return SUCCESS;
}


int calED(const string& query, const string& entry, int th)
//calculate ED between query and entry
{
	/*
	const char* t = query;
	int lent = strlen(t);
	const char* s = entry;
	int lens = strlen(s);
	*/
	const string& t = query;
	int lent = t.length();
	const string& s = entry;
	int lens = s.length();
	if(abs(lens - lent) > th)
		return MAX_NUM;
	if(lens > lent)
		return calED(entry, query, th);
		//when lens > lent swap entry and query to reach the complexity O(min(lens,lent)*(2*th+1))
	
	for (int i = 1; i <= lens; i++)
	{
		int lo = max(1,i-th);
		int hi = min(lent,i+th);
		//lower bound and upper bound
		bool flag = true;
		for (int j = lo; j <= hi; j++)
		//only calculate possible position using the threshold
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
		//early termination
	}
	return dis[lens][lent];
}

int SimJoiner::joinJaccard(const char *filename1, const char *filename2, double threshold, vector<JaccardJoinResult> &result) {
    result.clear();
    return SUCCESS;
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned threshold, vector<EDJoinResult> &result) {
    result.clear();
    readin(filename2);
    int index = 0;
    for(vector<string>::iterator i = line_vec.begin();i != line_vec.end();++i)
    {
    	vector<string> split_result;
    	split_gram(*i,qq,split_result,threshold);
    	//int overlap_th = i->length() + 1 - qq - threshold * qq;
    	for(vector<string>::iterator j = split_result.begin();j != split_result.end();++j)
    	{
    		map<string,int>::iterator find_iter = gram_set.find(*j);
    		if(find_iter == gram_set.end()) //not find
    		{
    			gram_set[*j] = gram_inverted_list.size();
    			gram_inverted_list.push_back(vector<int>());
    		}
    		gram_inverted_list[gram_set[*j]].push_back(index);
    	}
    	index++;
    }
    //cout << "build invert end\n";
    //set<pair<unsigned,unsigned>> candidate;
    ifstream fin(filename1);
	unsigned id = 0;
	string string_buf;
	while(!fin.eof())
	{
		getline(fin,string_buf);
		if(string_buf.size() == 0)
		{
			break;
		}
		int overlap_th = string_buf.size() + 1 - qq - threshold * qq;
		if(overlap_th > 0)
		{
			vector<string> split_result;
			split_gram(string_buf,qq,split_result,threshold);
			set<unsigned> candidate;
			for(vector<string>::iterator j = split_result.begin();j != split_result.end();++j)
	    	{
	    		map<string,int>::iterator find_iter = gram_set.find(*j);
	    		if(find_iter != gram_set.end()) // find
	    		{
	    			vector<int>& tmp_list = gram_inverted_list[gram_set[*j]];
	    			for(vector<int>::iterator k = tmp_list.begin();k != tmp_list.end();++k)
	    			{
	    				//candidate.insert(make_pair(id,(unsigned)*k));
	    				candidate.insert(*k);
	    				//cout << "pair : " << id << " " << *k << endl;
	    			}
	    		}
	    	}
	    	for(set<unsigned>::iterator it = candidate.begin();it != candidate.end();++it)
	    	{
	    		unsigned ed = (unsigned)calED(string_buf,line_vec[*it],threshold);
	    		if(ed <= threshold)
	    		{
	    			EDJoinResult r;
	    			r.id1 = (unsigned)id;
	    			r.id2 = *it;
	    			r.s = ed;
	    			//cout << r.id1 << " & " << r.id2 << " : " << r.s << endl;
	    			result.push_back(r);
	    		}
	    	}
    	}
    	else
    	{
    		int filename1_id = 0;
    		for(vector<string>::iterator it = line_vec.begin();it != line_vec.end();++it)
	    	{
	    		unsigned ed = (unsigned)calED(string_buf,*it,threshold);
	    		if(ed <= threshold)
	    		{
	    			EDJoinResult r;
	    			r.id1 = (unsigned)id;
	    			r.id2 = filename1_id;
	    			r.s = ed;
	    			//cout << r.id1 << " & " << r.id2 << " : " << r.s << endl;
	    			result.push_back(r);
	    		}
	    		filename1_id++;
	    	}
    	}
		id++;
	}
	fin.close();

    return SUCCESS;
}

void SimJoiner::printDebug()
{
	cout << "debug------------------------\n";
	for(vector<string>::iterator it = line_vec.begin();it != line_vec.end();++it)
	{
		cout << *it << "\n";
	}
	cout << "---------------" << endl;
	for(map<string,int>::iterator it = gram_set.begin();it != gram_set.end();++it)
	{
		int index = it->second;
		vector<int>& list = gram_inverted_list[index];
		cout << it->first << " : ";
		for(vector<int>::iterator it2 = list.begin();it2 != list.end();++it2)
		{
			cout << *it2 << " , ";
		}
		cout << endl;
	}
	cout << "end-------------------------\n";
}
