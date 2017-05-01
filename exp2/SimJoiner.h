#ifndef __EXP2_SIMJOINER_H__
#define __EXP2_SIMJOINER_H__

#include <vector>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>

template <typename IDType, typename SimType>
struct JoinResult {
    IDType id1;
    IDType id2;
    SimType s;
};

typedef JoinResult<unsigned, double> JaccardJoinResult;
typedef JoinResult<unsigned, unsigned> EDJoinResult;

const int SUCCESS = 0;
const int FAILURE = 1;

class SimJoiner {
public:
    SimJoiner();
    ~SimJoiner();

    int joinJaccard(const char *filename1, const char *filename2, double threshold, std::vector<JaccardJoinResult> &result);
    int joinED(const char *filename1, const char *filename2, unsigned threshold, std::vector<EDJoinResult> &result);

    void printDebug();
};

#endif
