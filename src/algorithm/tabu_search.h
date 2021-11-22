#ifndef __TABU_SEARCH_H__
#define __TABU_SEARCH_H__

#include "algorithm/solver.h"

namespace VRP {

class TabuSearch: public Solver
{
private:
	Solution best_solution;
private:
	static const int MAX_TIME_LIMIT = 400;
	static const int MAX_PERTURB_TIMES = 5;
	static const uint64_t MAX_BAD_ITERATION = 20000;
	virtual void main_process();
};

}

#endif
