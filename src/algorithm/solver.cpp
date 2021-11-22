#include <chrono>
#include <iostream>

#include "algorithm/solver.h"
#include "core/utility.h"

namespace VRP
{

Solution Solver::Work()
{
	main_clk = Clock();
	HeuristicGenerateSolution(solution);
	main_process();
	std::cout << "Elapse time: " << main_clk.GetHighPricisionDuration() << "ms." << std::endl;
	ConsoleShow(solution);
	FileShow(solution, std::to_string(main_clk.GetHighPricisionDuration()));
	return solution;
}

}
