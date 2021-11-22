#ifndef __RELOCATION_H__
#define __RELOCATION_H__

#include "core/operations.h"

class Relocation: public Operation
{
public:
	int customer_id;
	int route_id_1, route_id_2;
	int next_id;
	virtual Operation* operator()(Solution &solution);
	virtual void apply();
};

#endif
