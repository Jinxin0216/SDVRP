#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include <random>

#include "core/basetypes.h"
#include "core/operations.h"
#include "core/instance.h"

using std::string;

namespace VRP
{

cost_t get_dist(int id_1, int id_2);
#define INSERT_DIST(id_1, id_2, id_3) get_dist(id_1, id_2) + get_dist(id_2, id_3) - get_dist(id_1, id_3)
#define ERASE_DIST(id_1, id_2, id_3) get_dist(id_1, id_3) - get_dist(id_1, id_2) - get_dist(id_2, id_3)

void InitialTabuTable();

template<typename A, typename B>
cost_t get_distance(const A &p1, const B &p2)
{
	return hypot(p1.x - p2.x, p1.y - p2.y);
}

void InitializeSDVRPDatastruct();
void InitializeDistanceTable();

template <typename InstanceType>
void InitInstance(string filename)
{
	InstanceType::InputInstance(filename);
	InitializeSDVRPDatastruct();
	InitializeDistanceTable();
}

// Test whole solution's feasiblity
bool IsFeasible(const Solution &solution);

// Test part of solution's feasiblity(rooted by customer node)
bool IsFeasible(const Solution &solution, int customer_id);

cost_t GetCost(const Solution &solution);

Operation* HeuristicInsert(Solution &solution,
		int customer_id, int route_id,
		bool tabu_flag = false,
		bool first_flag = false,
		bool part_flag = false);
Operation* HeuristicInsert(Solution &solution,
		int customer_id,
		bool tabu_flag = false,
		bool first_flag = false,
		bool part_flag = false);

void HeuristicGenerateSolution(Solution &solution);

Operation* EnumerateRelocation(Solution &solution,
		bool tabu_flag = false,
		bool first_flag = false,
		bool part_flag = false);

void Perturb(Solution &solution, int kth);

const string RESULT_DIR = "../result/";
void ConsoleShow(Solution &solution);
void FileShow(Solution &solution, string comment = "Nan");

void Log();

};

#endif
