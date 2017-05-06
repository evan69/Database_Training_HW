#include "SimJoiner.h"

using namespace std;

#define getMin(a,b,c) (min(min(a,b),c))
// min of 3 num
#define MAX_NUM 1000000
// a large number
#define DIS_SIZE 4096

vector<string> line_vec;
//vector of lines in input file
vector<string> line_vec2;
//vector of lines in input file
int dis[4096][4096];
//used in DP calculation
/*
vector<vector<int> > gram_inverted_list;
//inverted list for ED
vector<vector<int> > gram_inverted_list_all;
//inverted list for ED
vector<vector<int> > word_inverted_list;
//inverted list for JAC
map<string,int> gram_set;
//set of gram for ED
map<string,int> word_set;
//set of word for JAC
*/
struct Segment
{
	unsigned segNo;
	map<string,int> invMap;
	vector<vector<int> > invList;
};

struct Group
{
	Segment segs[512];
	unsigned len;
	bool used = false;
};

Group partition_inverted_list[512];
Group partition_inverted_list2[512];

unsigned qq = 3;
unsigned tau;

SimJoiner::SimJoiner() {
	for (int i = 0; i < DIS_SIZE; i++)
		dis[i][0] = i;
	for (int j = 0; j < DIS_SIZE; j++)
		dis[0][j] = j;
}

SimJoiner::~SimJoiner() {
}

int readin(const char* filename1, const char *filename2)
// read in data
{
	ifstream fin(filename1);
	string string_buf;
	while(!fin.eof())
	{
		getline(fin,string_buf);
		if(string_buf.size() == 0)
		{
			break;
		}
		line_vec.push_back(string_buf);
	}
	
	fin.close();

	ifstream fin2(filename2);
	while(!fin2.eof())
	{
		getline(fin2,string_buf);
		if(string_buf.size() == 0)
		{
			break;
		}
		line_vec2.push_back(string_buf);
	}
	
	fin2.close();

	return SUCCESS;
}


int calED(const string& query, const string& entry, int th)
//calculate ED between query and entry
{
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

int split_partition(const string& str, unsigned len, vector<string>& result, unsigned offset)
{
	result.clear();
	unsigned i = 0;
	unsigned pos = offset;
	//unsigned len = str.length();
	unsigned length1 = floor(1.0 * len / (tau + 1));
	unsigned length2 = ceil(1.0 * len / (tau + 1));
	//assert(length1 > 0 && length2 > 0);
	unsigned num = len % (tau + 1);
	//assert(len == length2 * num + (tau + 1 - num) * length1);
	while(i < tau + 1)
	{
		unsigned real_len = (i < num) ? length2 : length1;
		string partition_entry = string(real_len,'*');
		for(unsigned j = 0;j < real_len;j++)
		{
			//assert(pos < str.length());
			partition_entry[j] = str[pos];
			pos ++;
		}
		i++;
 		result.push_back(partition_entry);
	}
	return SUCCESS;
}

int SimJoiner::joinJaccard(const char *filename1, const char *filename2, double threshold, vector<JaccardJoinResult> &result) {
    result.clear();
    return SUCCESS;
}

vector<int> lessTau;


bool cmp(const EDJoinResult& a,const EDJoinResult& b)
{
	if(a.id1 == b.id1)
		return a.id2 < b.id2;
	return a.id1 < b.id1;
}

bool operator==(const EDJoinResult& a,const EDJoinResult& b)
{
	return a.id1 == b.id1 && a.id2 == b.id2;
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned threshold, vector<EDJoinResult> &result) {
    result.clear();
    tau = threshold;
    readin(filename1,filename2);
    int index = 0;
    unsigned id;

    index = 0;
    for(vector<string>::iterator i = line_vec.begin();i != line_vec.end();++i)
    {
    	if(i->length() <= tau)
    	{
    		//assert(false);
    	}
    	vector<string> split_result;
    	split_partition(*i,i->length(),split_result,0);
    	//assert(split_result.size() == tau + 1);
    	Group& lengthGr = partition_inverted_list[i->length()];
    	lengthGr.used = true;
    	for(unsigned j = 0;j < split_result.size();j++)
    	{
    		string& substr = split_result[j];
    		Segment& seg = lengthGr.segs[j];
    		map<string,int>::iterator iter = seg.invMap.find(substr);
    		if(iter == seg.invMap.end()) //not find
    		{
    			seg.invMap[substr] = seg.invList.size();
    			seg.invList.push_back(vector<int>());
    		}
    		seg.invList[seg.invMap[substr]].push_back(index);
    	}
    	index++;
    }

	id = 0;
	while(id < line_vec2.size())
	{
		string string_buf = line_vec2[id];
		if(string_buf.length() == 0)
		{
			break;
		}
		
		set<int> candidate;
		if(string_buf.length() <= tau)
		{
			//assert(false);
		}
		unsigned lo = string_buf.length() - tau;
		unsigned hi = string_buf.length();
		for(unsigned len = lo;len <= hi;len++)
		{
			Group& lengthGr = partition_inverted_list[len];
			if(lengthGr.used == false)
				continue;
			for(unsigned offset = 0;offset <= string_buf.length() - len;offset++)
			{
				vector<string> split_result;
				split_partition(string_buf,len,split_result,offset);
				//assert(split_result.size() == tau + 1);
				for(unsigned index2 = 0;index2 < tau + 1;index2++)
				{
					Segment& segs = lengthGr.segs[index2];
					map<string,int>& invMap = segs.invMap;
					map<string,int>::iterator iter = invMap.find(split_result[index2]);
					if(iter != invMap.end())
					{
						unsigned a = invMap[split_result[index2]];
						vector<int>& vec = segs.invList[a];
						for(vector<int>::iterator it = vec.begin();it != vec.end();it++)
						{
							//assert(line_vec[*it].length() <= hi);
							candidate.insert(*it);
						}
					}
				}
			}
		}

		for(set<int>::iterator it = candidate.begin();it != candidate.end();++it)
		{
			unsigned ed = (unsigned)calED(string_buf,line_vec[*it],tau);
			if(ed <= tau)
			{
				EDJoinResult r;
				r.id2 = (unsigned)id;
				r.id1 = (unsigned)*it;
				r.s = ed;
				//cout << r.id1 << " & " << r.id2 << " : " << r.s << endl;
				result.push_back(r);
			}
		}
		id++;
	}

	//-------------------------------------

	index = 0;
    for(vector<string>::iterator i = line_vec2.begin();i != line_vec2.end();++i)
    {
    	if(i->length() <= tau)
    	{
    		//assert(false);
    	}
    	vector<string> split_result;
    	split_partition(*i,i->length(),split_result,0);
    	//assert(split_result.size() == tau + 1);
    	Group& lengthGr = partition_inverted_list2[i->length()];
    	lengthGr.used = true;
    	for(unsigned j = 0;j < split_result.size();j++)
    	{
    		string& substr = split_result[j];
    		Segment& seg = lengthGr.segs[j];
    		map<string,int>::iterator iter = seg.invMap.find(substr);
    		if(iter == seg.invMap.end()) //not find
    		{
    			seg.invMap[substr] = seg.invList.size();
    			seg.invList.push_back(vector<int>());
    		}
    		seg.invList[seg.invMap[substr]].push_back(index);
    	}
    	index++;
    }

	id = 0;
	while(id < line_vec.size())
	{
		string string_buf = line_vec[id];
		if(string_buf.length() == 0)
		{
			break;
		}
		
		set<int> candidate;
		if(string_buf.length() <= tau)
		{
			//assert(false);
		}
		unsigned lo = string_buf.length() - tau;
		unsigned hi = string_buf.length();
		for(unsigned len = lo;len < hi;len++)
		{
			Group& lengthGr = partition_inverted_list2[len];
			if(lengthGr.used == false)
				continue;
			for(unsigned offset = 0;offset <= string_buf.length() - len;offset++)
			{
				vector<string> split_result;
				split_partition(string_buf,len,split_result,offset);
				//assert(split_result.size() == tau + 1);
				for(unsigned index2 = 0;index2 < tau + 1;index2++)
				{
					Segment& segs = lengthGr.segs[index2];
					map<string,int>& invMap = segs.invMap;
					map<string,int>::iterator iter = invMap.find(split_result[index2]);
					if(iter != invMap.end())
					{
						unsigned a = invMap[split_result[index2]];
						vector<int>& vec = segs.invList[a];
						for(vector<int>::iterator it = vec.begin();it != vec.end();it++)
						{
							candidate.insert(*it);
						}
					}
				}
			}
		}

		for(set<int>::iterator it = candidate.begin();it != candidate.end();++it)
		{
			unsigned ed = (unsigned)calED(string_buf,line_vec2[*it],tau);
			if(ed <= tau)
			{
				EDJoinResult r;
				r.id1 = (unsigned)id;
				r.id2 = (unsigned)*it;
				r.s = ed;
				//cout << r.id1 << " & " << r.id2 << " : " << r.s << endl;
				result.push_back(r);
			}
		}
		id++;
	}

	sort(result.begin(), result.end(), cmp);
	result.erase(unique(result.begin(),result.end()),result.end());

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

	for(int i = 0;i < 500;i++)
	{
		Group& lengthGr = partition_inverted_list[i];
		if(lengthGr.used == false)
			continue;
		cout << "length : " << i <<  endl;
		for(unsigned j = 0;j < tau + 1;j++)
		{
			Segment& segs = lengthGr.segs[j];
			for(map<string,int>::iterator iter = segs.invMap.begin();iter != segs.invMap.end();iter++)
			{
				cout << iter->first << ": \n";
				unsigned a = iter->second;
				vector<int>& vec = segs.invList[a];
				for(vector<int>::iterator it = vec.begin();it != vec.end();it++)
				{
					cout << *it << endl;
				}
			}
		}

	}


	cout << "---------------" << endl;
	/*
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
	*/
	cout << "end-------------------------\n";
}
