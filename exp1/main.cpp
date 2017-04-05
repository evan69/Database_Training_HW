#include "SimSearcher.h"

using namespace std;

int main(int argc, char **argv)
{
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 2, edThreshold = 1;
	double jaccardThreshold = 0.85;

	searcher.createIndex(argv[1], q);
	//searcher.searchJaccard("query", jaccardThreshold, resultJaccard);
	searcher.searchED("shtick", edThreshold, resultED);

	searcher.printDebug(resultED);
	return 0;
}

