#include <cassert>

#include "core/instance.h"

namespace VRP
{
Vehicle vehicle;
vector<Customer> customers;
vector<Depot> depots;
string instance_name;

void SDVRP::InputInstance(string filename)
{
	std::size_t begin_pos, end_pos;
	begin_pos = filename.find_last_of('/');
	if (begin_pos == -1) {
		begin_pos = filename.find_last_of('\\');
	}
	end_pos = filename.find_last_of('.');
	instance_name = filename.substr(begin_pos + 1, end_pos - begin_pos - 1);
	FILE *fp = fopen(filename.c_str(), "r");
	int customer_num;
	assert(fscanf(fp, "%d %d", &customer_num, &vehicle.capacity) == 2);
	customers.resize(customer_num);
	for (int i = 0; i < customer_num; i++) {
		assert(fscanf(fp, "%d", &customers[i].demand) == 1);
	}
	depots.resize(1);
	assert(fscanf(fp, "%d %d", &depots[0].x, &depots[0].y) == 2);
	for (int i = 0; i < customer_num; i++) {
		assert(fscanf(fp, "%d %d", &customers[i].x, &customers[i].y) == 2);
	}
	fclose(fp);
}

}
