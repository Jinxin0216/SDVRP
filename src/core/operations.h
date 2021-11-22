#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#include <functional>

#include "core/basetypes.h"
#include "core/relocation.h"

namespace VRP {

class Operation
{
public:
	bool tabu_flag;
	Solution *ptr;
	cost_t cost;
	cost_t virtual_cost;
	virtual Operation* operator()(Solution &solution) { return NULL; }
	virtual void apply() = 0;
};

class Insert: public Operation
{
public:
	int customer_id;
	int route_id;
	int next_id;
	virtual void apply();
};

class Exchange: public Operation
{
public:
	int customer_id_1, customer_id_2;
	int route_id_1, route_id_2;
	virtual void apply();
};

class Reverse: public Operation
{
public:
	int customer_id_1, customer_id_2;
	int route_id;
	virtual void apply();
};

class Cross: public Operation
{
public:
	int customer_id_1, customer_id_2, next_id_1;
	int customer_id_3, customer_id_4, next_id_2;
	int route_id_1, route_id_2;
	bool reverse_flag_1, reverse_flag_2;
	virtual void apply();
};

class Split: public Operation
{
public:
	int customer_id;
	vector<Insert> inserts;
	virtual void apply();
};

}

#endif
