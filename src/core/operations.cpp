#include <cstdio>
#include <cassert>

#include <algorithm>

#include "core/operations.h"
#include "core/utility.h"
#include "core/instance.h"

namespace VRP
{
extern vector<vector<uint64_t>> remove_tabu_table;
extern vector<vector<uint64_t>> insert_tabu_table;
extern vector<uint64_t> reverse_tabu_table;
extern uint64_t iteration;
uint64_t tabu_step;

inline void update_tabu_step(Solution *ptr)
{
	tabu_step = std::pow(customers.size(), .5);
}

bool is_accept(cost_t current_cost, Operation* previous_op)
{
	return dcmp(current_cost)
		&& (previous_op == NULL || current_cost < previous_op->cost);
}

void Insert::apply()
{
	ptr->Insert(customer_id, route_id, next_id);
	ptr->cost += cost;
	if (tabu_flag) {
		update_tabu_step(ptr);
		remove_tabu_table[customer_id][route_id] = iteration + tabu_step;
	}
}

Operation* Relocation::operator () (Solution &solution)
{
	Clock clk;
	//puts("EnumerateRelocation begin");
	Operation* op = NULL;
	ptr = &solution;
	tabu_flag = true;

	//puts("EnumerateRelocation end");
	return op;
}

void Relocation::apply()
{
	ptr->Erase(customer_id, route_id_1);
	ptr->Insert(customer_id, route_id_2, next_id);
	ptr->cost += cost;
	if (tabu_flag) {
		update_tabu_step(ptr);
		remove_tabu_table[customer_id][route_id_2] = iteration + tabu_step;
		insert_tabu_table[customer_id][route_id_1] = iteration + tabu_step;
	}
}

void Exchange::apply()
{
	int previous_id_1 = ptr->route_nodes[route_id_1].GetPreviousID(customer_id_1);
	int previous_id_2 = ptr->route_nodes[route_id_2].GetPreviousID(customer_id_2);
	int next_id_1 = ptr->Erase(customer_id_1, route_id_1);
	int next_id_2 = ptr->Erase(customer_id_2, route_id_2);
	ptr->Insert(customer_id_2, route_id_1, next_id_1);
	ptr->Insert(customer_id_1, route_id_2, next_id_2);
	ptr->cost += cost;
	if (tabu_flag) {
		update_tabu_step(ptr);
		if (route_id_1 != route_id_2) {
			remove_tabu_table[customer_id_1][route_id_2] = iteration + tabu_step;
			remove_tabu_table[customer_id_2][route_id_1] = iteration + tabu_step;
			insert_tabu_table[customer_id_1][route_id_1] = iteration + tabu_step;
			insert_tabu_table[customer_id_2][route_id_2] = iteration + tabu_step;
		}
	}
	bool feasible_flag_1 = IsFeasible(*ptr, customer_id_1);
	bool feasible_flag_2 = IsFeasible(*ptr, customer_id_2);
	if (feasible_flag_1 && feasible_flag_2) {
	} else if (!feasible_flag_1) {
		if (INSERT_DIST(previous_id_1, customer_id_1, customer_id_2) <
			INSERT_DIST(customer_id_2, customer_id_1, next_id_1)) {
			ptr->Insert(customer_id_1, route_id_1, customer_id_2);
		} else {
			ptr->Insert(customer_id_1, route_id_1, next_id_1);
		}
	} else if (!feasible_flag_2) {
		if (INSERT_DIST(previous_id_2, customer_id_2, customer_id_1) <
			INSERT_DIST(customer_id_1, customer_id_2, next_id_2)) {
			ptr->Insert(customer_id_2, route_id_2, customer_id_1);
		} else {
			ptr->Insert(customer_id_2, route_id_2, next_id_2);
		}
	}
}

void Reverse::apply()
{
	ptr->route_nodes[route_id].Reverse(customer_id_1, customer_id_2);
	ptr->cost += cost;
	if (tabu_flag) {
		update_tabu_step(ptr);
		reverse_tabu_table[route_id] = iteration + tabu_step;
	}
}

void Cross::apply()
{
	std::vector<int> tmp_vec_1, tmp_vec_2;
	if (~customer_id_1) {
		auto begin_position = ptr->GetPosition(route_id_1, customer_id_1);
		auto end_position = ++ptr->GetPosition(route_id_1, customer_id_2);
		for (Position position = begin_position;
			 position != end_position; ) {
			int customer_id = *position++;
			ptr->Erase(customer_id, route_id_1);
			tmp_vec_1.push_back(customer_id);
		}
	}
	if (~customer_id_3) {
		auto begin_position = ptr->GetPosition(route_id_2, customer_id_3);
		auto end_position = ++ptr->GetPosition(route_id_2, customer_id_4);
		for (Position position = begin_position;
			 position != end_position; ) {
			int customer_id = *position++;
			ptr->Erase(customer_id, route_id_2);
			tmp_vec_2.push_back(customer_id);
		}
	}
	if (reverse_flag_1) {
		std::reverse(tmp_vec_1.begin(), tmp_vec_1.end());
	}
	if (reverse_flag_2) {
		std::reverse(tmp_vec_2.begin(), tmp_vec_2.end());
	}
	for (int customer_id: tmp_vec_2) {
		ptr->Insert(customer_id, route_id_1, next_id_1);
		if (tabu_flag && route_id_1 != route_id_2) {
			update_tabu_step(ptr);
			insert_tabu_table[customer_id][route_id_2] = iteration + tabu_step;
			remove_tabu_table[customer_id][route_id_1] = iteration + tabu_step;
		}
	}
	for (int customer_id: tmp_vec_1) {
		ptr->Insert(customer_id, route_id_2, next_id_2);
		if (tabu_flag && route_id_1 != route_id_2) {
			update_tabu_step(ptr);
			insert_tabu_table[customer_id][route_id_1] = iteration + tabu_step;
			remove_tabu_table[customer_id][route_id_2] = iteration + tabu_step;
		}
	}
	ptr->cost += cost;
	assert(dcmp(ptr->cost - GetCost(*ptr)) == 0);
}

void Split::apply()
{
	auto serving_route = ptr->customer_nodes[customer_id].GetRoute();
	for (int route_id: serving_route) {
		if (tabu_flag && iteration < remove_tabu_table[customer_id][route_id]) continue;
		if (tabu_flag) {
			update_tabu_step(ptr);
			insert_tabu_table[customer_id][route_id] = iteration + tabu_step;
		}
		ptr->customer_nodes[customer_id].Erase(route_id);
		ptr->route_nodes[route_id].Erase(customer_id);
	}
	for (Insert insert: inserts) {
		insert.apply();
	}
	ptr->cost = GetCost(*ptr);
}

}
