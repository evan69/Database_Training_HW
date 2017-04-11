#include "SimSearcher.h"

using namespace std;

int main(int argc, char **argv)
{
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 2, edThreshold = 1;
	double jaccardThreshold = 0.0;

	searcher.createIndex(argv[1], q);
	//searcher.searchJaccard("query", jaccardThreshold, resultJaccard);
	searcher.searchED("shtick", edThreshold, resultED);
	searcher.printDebug(resultED);
	searcher.searchED("srick", edThreshold, resultED);
	searcher.printDebug(resultED);
	searcher.searchJaccard("bb bbb", jaccardThreshold, resultJaccard);
	cout << "JAC000\n";

	for(vector<pair<unsigned, double> >::iterator it = resultJaccard.begin();it != resultJaccard.end();++it)
	{
		cout << it->first << ":" << " jac: " << it->second << endl;
	}

	cout << "end-------------------------\n";
	return 0;
}

