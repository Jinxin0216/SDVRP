#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include <string>

#include "core/basetypes.h"

using std::string;

namespace VRP
{

extern Vehicle vehicle;
extern vector<Customer> customers;
extern vector<Depot> depots;
extern string instance_name;

class SDVRP {
public:
	static void InputInstance(string filename);
};

}

#endif
