#include <cassert>
#include <cmath>
#include <cstddef>

#include <algorithm>
#include <iostream>

#include "core/utility.h"
#include "core/instance.h"

namespace VRP {

const int MAX_CROSS_LENGTH = 1000;

double random_propability;

int route_num;
vector<vector<cost_t>> dist_depot2customer;
vector<vector<cost_t>> dist_customer2customer;

typedef uint64_t visit_t;
vector<visit_t> visit_route;
vector<visit_t> visit_customer;
vector<int> residual_capacity;
vector<int> residual_demand;
visit_t clk;

vector<vector<int>> serving_amounts;

typedef uint64_t iteration_t;
iteration_t iteration;
vector<vector<iteration_t>> insert_tabu_table;
vector<vector<iteration_t>> remove_tabu_table;
vector<iteration_t> reverse_tabu_table;

cost_t get_dist(int id_1, int id_2) {
	if (id_1 < 0 && id_2 < 0) {
		return 0;
	} else if (id_1 < 0) {
		return dist_depot2customer[-id_1 - 1][id_2];
	} else if (id_2 < 0) {
		return dist_depot2customer[-id_2 - 1][id_1];
	} else {
		return dist_customer2customer[id_1][id_2];
	}
}

void InitialTabuTable()
{
	iteration = 0;
	fix(remove_tabu_table, customers.size(), route_num);
	fix(insert_tabu_table, customers.size(), route_num);
	fix(reverse_tabu_table, route_num);
}

void InitializeSDVRPDatastruct()
{
	route_num = customers.size();
	clean(visit_route, route_num);
	clean(residual_capacity, route_num);
	clean(visit_customer, customers.size());
	clean(residual_demand, customers.size());
	clk = 0;
}

void InitializeDistanceTable()
{
	dist_depot2customer.resize(depots.size());
	for (size_t i = 0; i < depots.size(); i++) {
		dist_depot2customer[i].resize(customers.size());
		for (size_t j = 0; j < customers.size(); j++) {
			dist_depot2customer[i][j] = get_distance(depots[i], customers[j]);
		}
	}
	dist_customer2customer.resize(customers.size());
	for (size_t i = 0; i < customers.size(); i++) {
		dist_customer2customer[i].resize(customers.size());
		for (size_t j = 0; j < customers.size(); j++) {
			dist_customer2customer[i][j] = get_distance(customers[i], customers[j]);
		}
	}
}

bool loop_flag;
bool demand_fulfill_flag;
void dfs_route_node(const Solution &solution, int route_id, int previous_id);
void dfs_customer_node(const Solution &solution, int customer_id, int previous_id);
bool output_flag = false;

void dfs_route_node(const Solution &solution, int route_id, int previous_id = -1)
{
	visit_route[route_id] = clk;
	residual_capacity[route_id] = vehicle.capacity;
	for (const int customer_id: solution.route_nodes[route_id]) {
		if (customer_id == previous_id) continue;
		if (visit_customer[customer_id] == clk) {
			loop_flag = true;
			return;
		}
		dfs_customer_node(solution, customer_id, route_id);
		if (loop_flag) return;
		if (residual_demand[customer_id] > residual_capacity[route_id]) {
			demand_fulfill_flag = false;
		} else {
			if (output_flag) {
				serving_amounts[route_id][customer_id] = residual_demand[customer_id];
			}
			residual_capacity[route_id] -= residual_demand[customer_id];
			residual_demand[customer_id] = 0;
		}
	}
}

void dfs_customer_node(const Solution &solution, int customer_id, int previous_id = -1)
{
	visit_customer[customer_id] = clk;
	residual_demand[customer_id] = customers[customer_id].demand;
	for (int route_id: const_cast<CustomerNode&>(solution.customer_nodes[customer_id])) {
		if (route_id == previous_id) continue;
		if (visit_route[route_id] == clk) {
			loop_flag = true;
			return;
		}
		dfs_route_node(solution, route_id, customer_id);
		if (loop_flag) return;
		int serving_amount = std::min(residual_capacity[route_id], residual_demand[customer_id]);
		if (output_flag) {
			serving_amounts[route_id][customer_id] = serving_amount;
		}
		residual_capacity[route_id] -= serving_amount;
		residual_demand[customer_id] -= serving_amount;
	}
}

bool IsFeasible(const Solution &solution)
{
	clk++;
	loop_flag = false;
	demand_fulfill_flag = true;
	for (int customer_id = 0; customer_id < int(customers.size()); customer_id++) {
		if (visit_customer[customer_id] != clk) {
			dfs_customer_node(solution, customer_id);
			if (loop_flag) return false;
			if (residual_demand[customer_id]) {
				demand_fulfill_flag = false;
			}
		}
	}
	return demand_fulfill_flag;
}

bool IsFeasible(const Solution &solution, int customer_id)
{
	clk++;
	loop_flag = false;
	demand_fulfill_flag = true;
	dfs_customer_node(solution, customer_id);
	if (residual_demand[customer_id]) {
		demand_fulfill_flag = false;
	}
	return !loop_flag && demand_fulfill_flag;
}

bool IsFeasibleR(const Solution &solution, int route_id)
{
	clk++;
	loop_flag = false;
	demand_fulfill_flag = true;
	dfs_route_node(solution, route_id);
	return !loop_flag && demand_fulfill_flag;
}

cost_t GetCost(const Solution &solution)
{
	cost_t ret = 0;
	for (auto &route_node: solution.route_nodes) {
		int previous_id = -1;
		for (int customer_id: route_node) {
			ret += get_dist(previous_id, customer_id);
			previous_id = customer_id;
		}
		ret += get_dist(previous_id, -1);
	}
	return ret;
}

void ConsoleShow(Solution &solution)
{
	int idx = 0;
	printf("instance: %s\n", instance_name.c_str());
#ifdef DEBUG
	for (int route_id = 0; route_id < route_num; route_id++) {
		if (solution.route_nodes[route_id].Empty()) continue;
		if (++idx < 20) {
			printf("%d:", idx);
			for (int customer_id: solution.route_nodes[route_id]) {
				printf(" %d", customer_id);
			}
			puts("");
		}
	}
	if (idx > 20) puts("...");
	assert(dcmp(GetCost(solution) - solution.cost) == 0);
#endif
	printf("Total # of vehicle: %d\n", solution.GetUsedRouteCount());
	printf("Cost: %f %f\n", GetCost(solution), solution.cost);
	printf("Fesiable: %d\n", IsFeasible(solution));
	puts("");
}

void FileShow(Solution &solution, string comment)
{
	output_flag = true;
	serving_amounts.resize(route_num);
	for (int i = 0; i < route_num; i++) {
		serving_amounts[i].resize(customers.size());
	}
	IsFeasible(solution);
	output_flag = false;

	string filename = RESULT_DIR + instance_name + "_" + std::to_string(solution.cost) + ".result";
	FILE* fp = fopen(filename.c_str(), "w");
	int idx = 0;
	fprintf(fp, "num_of_route:%d\n", solution.GetUsedRouteCount());
	for (int route_id = 0; route_id < route_num; route_id++) {
		if (solution.route_nodes[route_id].Empty()) continue;
		fprintf(fp, "%d:", ++idx);
		for (int customer_id: solution.route_nodes[route_id]) {
			fprintf(fp, " %d|%d", customer_id, serving_amounts[route_id][customer_id]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "cost:%f", solution.cost);
	fclose(fp);
	filename = RESULT_DIR + "summary.result";
	fp = fopen(filename.c_str(), "a");
	assert(dcmp(solution.cost - GetCost(solution)) == 0);
	fprintf(fp, "%s %f %d %s\n", instance_name.c_str(), solution.cost, idx, comment.c_str());
	fclose(fp);
}

Insert insert_operation;
void heuristic_insert(Solution &solution, int customer_id, int route_id)
{
	RouteNode &route_node = solution.route_nodes[route_id];
	CPosition position;
	cost_t cost_1, cost_2;
	cost_t cost;
	if (route_node.IsServing(customer_id)) {
		int previous_id = route_node.GetPreviousID(customer_id);
		int next_id = route_node.GetNextID(customer_id);
		cost_1 = ERASE_DIST(previous_id, customer_id, next_id);
	} else {
		cost_1 = 0;
	}
	int previous_id = -1;
	for (int next_id: route_node) {
		if (next_id == customer_id) continue;
		cost_2 = INSERT_DIST(previous_id, customer_id, next_id);
		cost = cost_1 + cost_2;
		if (cost < insert_operation.cost) {
			insert_operation.cost = insert_operation.virtual_cost = cost;
			insert_operation.customer_id = customer_id;
			insert_operation.route_id = route_id;
			insert_operation.next_id = next_id;
		}
		previous_id = next_id;
	}
	cost_2 = INSERT_DIST(previous_id, customer_id, -1);
	cost = cost_1 + cost_2;
	if (cost < insert_operation.cost) {
		insert_operation.cost = insert_operation.virtual_cost = cost;
		insert_operation.customer_id = customer_id;
		insert_operation.route_id = route_id;
		insert_operation.next_id = -1;
	}
}

Operation* HeuristicInsert(Solution &solution,
						   int customer_id, int route_id,
						   bool tabu_flag,
						   bool first_flag,
						   bool part_flag)
{
	insert_operation.ptr = &solution;
	insert_operation.tabu_flag = tabu_flag;
	insert_operation.cost = insert_operation.virtual_cost = std::numeric_limits<cost_t>::max();
	heuristic_insert(solution, customer_id, route_id);
	return &insert_operation;
}

Operation* HeuristicInsert(Solution &solution,
						   int customer_id,
						   bool tabu_flag,
						   bool first_flag,
						   bool part_flag)
{
	insert_operation.ptr = &solution;
	insert_operation.tabu_flag = tabu_flag;
	insert_operation.cost = insert_operation.virtual_cost = std::numeric_limits<cost_t>::max();
	for (int route_id = 0; route_id < route_num; route_id++) {
		heuristic_insert(solution, customer_id, route_id);
	}
	return &insert_operation;
}

Operation* generate_heuristic_insert(Solution &solution,
									 int customer_id)
{
	insert_operation.ptr = &solution;
	insert_operation.tabu_flag = false;
	insert_operation.cost = insert_operation.virtual_cost = std::numeric_limits<cost_t>::max();
	int min_route_num = 0;
	for (auto x: customers) min_route_num += x.demand;
	min_route_num = (min_route_num + vehicle.capacity - 1) / vehicle.capacity;
	int first_empty_route_id = -1;
	bool check_empty_flag = solution.GetUsedRouteCount() >= min_route_num;
	for (int route_id = 0; route_id < route_num; route_id++) {
		if (check_empty_flag && solution.route_nodes[route_id].Empty()) {
			if (~first_empty_route_id) {
				continue;
			} else {
				first_empty_route_id = route_id;
			}
		}
		else if (residual_capacity[route_id]) {
			heuristic_insert(solution, customer_id, route_id);
		}
	}
	if (insert_operation.cost == std::numeric_limits<cost_t>::max()) {
		heuristic_insert(solution, customer_id, first_empty_route_id);
	}
	return &insert_operation;
}

void HeuristicGenerateSolution(Solution &solution)
{
	solution.Initialize(customers.size(), route_num);

	for (int route_id = 0; route_id < route_num; route_id++) {
		residual_capacity[route_id] = vehicle.capacity;
	}
	vector<int> customer_ids = gen_range_numbers(customers.size());
	// std::random_shuffle(customer_ids.begin(), customer_ids.end());

	for (int customer_id: customer_ids) {
		residual_demand[customer_id] = customers[customer_id].demand;
		while (residual_demand[customer_id]) {
			Insert* op = dynamic_cast<Insert*>(generate_heuristic_insert(solution, customer_id));
			assert(op->cost != std::numeric_limits<cost_t>::max());
			int serving_amount = std::min(residual_demand[customer_id], residual_capacity[op->route_id]);
			residual_demand[customer_id] -= serving_amount;
			residual_capacity[op->route_id] -= serving_amount;
			op->apply();
		}
	}
}

Operation* EnumerateRelocation(Solution &solution,
							   bool tabu_flag,
							   bool first_flag,
							   bool part_flag)
{
}

void Perturb(Solution &solution, int kth)
{
	vector<int> customer_ids;
	for (int customer_id = 0; customer_id < int(customers.size()); customer_id++) {
		customer_ids.push_back(customer_id);
	}
	std::random_shuffle(customer_ids.begin(), customer_ids.end());
	int sz = random_generate(3, 5);
	if (sz < customer_ids.size())
		customer_ids.resize(sz);
	for (int customer_id: customer_ids) {
		auto serving_route = solution.customer_nodes[customer_id].GetRoute();

		for (int route_id: serving_route) {
			solution.Erase(customer_id, route_id);
		}
	}

	for (int customer_id: customer_ids) {
		vector<std::pair<cost_t, int>> px;
		vector<int> empty_route_ids;
		for (int route_id = 0; route_id < route_num; route_id++) {
			if (solution.route_nodes[route_id].Empty()) {
				empty_route_ids.push_back(route_id);
				continue;
			}
			clk++;
			dfs_route_node(solution, route_id);
			Operation* op = HeuristicInsert(solution,
											customer_id, route_id,
											false, false, false);
			if (residual_capacity[route_id] > 0) {
				px.emplace_back(op->cost / residual_capacity[route_id], route_id);
			}
		}
		std::sort(px.begin(), px.end());
		for (int route_id: empty_route_ids) {
			px.emplace_back(0, route_id);
		}

		for (size_t i = 0; i < px.size(); i++) {
			int route_id = px[i].second;
			HeuristicInsert( solution, customer_id, route_id)->apply();
			if (IsFeasible(solution, customer_id)) {
				break;
			}
			if (loop_flag) {
				solution.Erase(customer_id, route_id);
			}
		}
	}
	assert(IsFeasible(solution));
	solution.cost = GetCost(solution);
	return;
}

void Log()
{
}

}
