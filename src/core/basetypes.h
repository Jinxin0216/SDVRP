#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

#include <list>
#include <unordered_map>

#include "core/helper.h"

using std::list;
using std::unordered_map;

namespace VRP {

typedef double cost_t;

struct Vehicle
{
	int capacity;
};

struct Customer
{
	int x, y;
	int demand;
};

struct Depot
{
	int x, y;
};

typedef list<int>::iterator Position;
typedef list<int>::const_iterator CPosition;
const Position null_position;

class CustomerNode
{
protected:
	vector<Position> positions;
	list<int> serving_route;
public:
	CustomerNode &operator = (const CustomerNode &other);
	void Insert(int route_id);
	void Erase(int route_id);
	bool IsServedBy(int route_id);
	list<int> GetRoute() { return serving_route; }
	friend CPosition begin(const CustomerNode &customer_node);
	friend CPosition end(const CustomerNode &customer_node);
};

class RouteNode
{
protected:
#define DEPOT_ID (-depot_id - 1)
	int depot_id = 0;
	vector<Position> positions;
	list<int> served_customer;
public:
	RouteNode &operator = (const RouteNode &other);
	void Insert(int customer_id, int next_id);
	int Erase(int customer_id);
	bool IsServing(int customer_id);
	int GetPreviousID(int customer_id);
	int GetNextID(int customer_id);
	void Reverse(int customer_id_1, int customer_id_2);
	bool Empty() { return served_customer.empty(); }
	int GetDepotID() const { return DEPOT_ID; }
	list<int> GetRoute() { return served_customer; }
	Position GetPosition(int customer_id) { return positions[customer_id]; }
	friend CPosition begin(const RouteNode &route_node);
	friend CPosition end(const RouteNode &route_node);
};

class Solution
{
private:
#define C(customer_id) (customer_id)
#define R(route_id) (route_id + int(customer_nodes.size()))
	vector<vector<int>> pre, post; // pre[x][y] / post[x][y]
	vector<int> tree_id; // tree_id[x]
	vector<int> alpha; // alpha[rj]
	vector<int> beta_sum; // beta_sum[ci]
	vector<unordered_map<int, int>> beta; // beta[ci][rj]
	vector<vector<unordered_map<int, int>>> theta; // theta[x][rj][ci]
	vector<vector<int>> gamma; // gamma[rj][rk](x)
	vector<vector<int>> delta; // delta[ci][ck](x)
public:
	vector<CustomerNode> customer_nodes;
	vector<RouteNode> route_nodes;
	cost_t cost;
	Solution &operator = (const Solution &solution);
	void Prepare();
	void Initialize(size_t customer_size, size_t route_size);
	int GetUsedRouteCount();
	int Erase(int customer_id, int route_id);
	void Insert(int customer_id, int route_id, int next_id);
	int GetPreviousID(int route_id, int customer_id) {
		return route_nodes[route_id].GetPreviousID(customer_id);
	}
	int GetNextID(int route_id, int customer_id) {
		return route_nodes[route_id].GetNextID(customer_id);
	}
	bool IsServing(int route_id, int customer_id) {
		return route_nodes[route_id].IsServing(customer_id);
	}
	Position GetPosition(int route_id, int customer_id) {
		return route_nodes[route_id].GetPosition(customer_id);
	}
	list<int> GetRoute(int route_id) { return route_nodes[route_id].GetRoute(); }
	vector<list<int>> GetRoutes() {
		vector<list<int>> ret;
		for (auto &x: route_nodes) {
			ret.push_back(x.GetRoute());
		}
		return ret;
	}
};

}

#endif
