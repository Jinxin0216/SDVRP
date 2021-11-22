#include <algorithm>
#include <chrono>
#include <functional>

#include "algorithm/tabu_search.h"
#include "core/utility.h"
#include "core/instance.h"

namespace VRP {

extern double random_propability;
extern uint64_t iteration;
extern int route_num;

void TabuSearch::main_process()
{
	best_solution = solution;
	InitialTabuTable();

	vector<Operation*> nbr_ops;
	nbr_ops.push_back(new Relocation);

	Clock clk;
	uint64_t bad_iteration = 0;
	uint64_t perturb_times = 0;
	uint64_t max_bad_iteration = std::pow(customers.size(), 3);
	if (max_bad_iteration > MAX_BAD_ITERATION) max_bad_iteration = MAX_BAD_ITERATION;
	ECHO(max_bad_iteration);
	bool perturb_flag = false;
	for (;;) {
		random_propability = 1. / std::pow(customers.size(),
										   .5 - .1 * (perturb_times / MAX_PERTURB_TIMES));
		auto elapse_time = clk.GetDuration();
		if (elapse_time >= MAX_TIME_LIMIT) break;
		Operation* best_op = NULL;
		for (auto nbr_op_ptr: nbr_ops) {
			auto &nbr_op = *nbr_op_ptr;
			Operation* op = nbr_op(solution);
			if (op == NULL)  {
				continue;
			}
			if (best_op == NULL || op->cost < best_op->cost) {
				best_op = op;
			}
		}
		if (best_op && best_op->cost != std::numeric_limits<cost_t>::max()) {
			best_op->apply();
		}

		//printf("%f\n", solution.cost);
		if (dcmp(solution.cost - best_solution.cost) < 0) {
			bad_iteration = 0;
			best_solution = solution;
		} else {
			bad_iteration++;
		}
		if (bad_iteration == max_bad_iteration) {
			if (++perturb_times > MAX_PERTURB_TIMES) {
				break;
			}
			printf("iteration %lu: cost(%f) route_num(%d)\n", perturb_times, best_solution.cost, best_solution.GetUsedRouteCount());
			bad_iteration = 0;
			solution = best_solution;
			Perturb(solution, perturb_times);
			if (dcmp(solution.cost - best_solution.cost)) {
				perturb_flag = true;
			}
			InitialTabuTable();
		} else {
			iteration++;
		}
	}
	solution = best_solution;
	Log();
}

};
