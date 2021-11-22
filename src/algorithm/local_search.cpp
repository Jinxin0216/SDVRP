#include <algorithm>
#include <chrono>
#include <functional>

#include "algorithm/local_search.h"
#include "core/utility.h"

namespace VRP
{

extern double random_propability;

extern int route_num;

void LocalSearch::main_process()
{
	random_propability = 1. / route_num;

	/*
	org_nbr_ops.emplace_back(EnumerateRelocation);
	org_nbr_ops.emplace_back(EnumerateExchange);
	org_nbr_ops.emplace_back(EnumerateReverse);
	org_nbr_ops.emplace_back(EnumerateSplit);

	bool part_flag = false;
	Clock clk;
	for (iteration = 0; ; iteration++) {
		auto elapse = clk.GetDuration();
		if (elapse >= MAX_TIME_LIMIT) break;
		Operation* best_op = NULL;
		for (OriginalNeighborOperation org_nbr_op: org_nbr_ops) {
			auto nbr_op = std::bind(org_nbr_op, std::placeholders::_1, false, false, part_flag);
			Operation* op = nbr_op(solution);
			if (op == NULL)  {
				continue;
			}
			if (best_op == NULL || op->cost < best_op->cost) {
				best_op = op;
			}
		}
		if (best_op && dcmp(best_op->cost) < 0) {
			best_op->apply();
		} else if (part_flag) {
			part_flag = false;
		} else {
			break;
		}
	}
	*/
}

};
