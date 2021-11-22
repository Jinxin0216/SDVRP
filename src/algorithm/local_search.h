#ifndef __LOCAL_SEARCH__
#define __LOCAL_SEARCH__

#include "algorithm/solver.h"

namespace VRP
{

extern uint64_t iteration;

class LocalSearch: public Solver
{
private:
	static const int MAX_TIME_LIMIT = 200;
	virtual void main_process();
};

};

#endif
