#include "core/basetypes.h"
#include <cassert>

namespace VRP {

CustomerNode &CustomerNode::operator = (const CustomerNode &other)
{
	serving_route = other.serving_route;
	clean(positions, other.positions.size());
	for (Position position = begin(serving_route);
			position != end(serving_route);
			position++) {
		positions[*position] = position;
	}
	for (int i = 0; i < int(positions.size()); i++) {
		if (positions[i] == null_position) continue;
		assert(*positions[i] == i);
	}
	return *this;
}

void CustomerNode::Insert(int route_id)
{
	fix(positions, route_id + 1);
	assert(positions[route_id] == null_position);
	positions[route_id] = serving_route.insert(
			serving_route.end(), route_id);
}

void CustomerNode::Erase(int route_id)
{
	assert(positions[route_id] != null_position);
	serving_route.erase(positions[route_id]);
	positions[route_id] = null_position;
}

bool CustomerNode::IsServedBy(int route_id)
{
	return route_id < positions.size()
		&& positions[route_id] != null_position;
}

CPosition begin(const CustomerNode &customer_node)
{
	return customer_node.serving_route.cbegin();
}

CPosition end(const CustomerNode &customer_node)
{
	return customer_node.serving_route.cend();
}

RouteNode &RouteNode::operator = (const RouteNode &other)
{
	served_customer = other.served_customer;
	clean(positions, other.positions.size());
	for (Position position = begin(served_customer);
			position != end(served_customer);
			position++) {
		positions[*position] = position;
	}
	return *this;
}


void RouteNode::Insert(int customer_id, int next_id)
{
	fix(positions, customer_id + 1);
	assert(positions[customer_id] == null_position);
	positions[customer_id] = served_customer.insert(
			next_id < 0? served_customer.end(): positions[next_id],
			customer_id);
}

int RouteNode::Erase(int customer_id)
{
	assert(positions[customer_id] != null_position);
	Position position = served_customer.erase(positions[customer_id]);
	positions[customer_id] = null_position;
	return position == served_customer.end()? DEPOT_ID: *position;
}

bool RouteNode::IsServing(int customer_id)
{
	return customer_id < positions.size()
		&& positions[customer_id] != null_position;
}

int RouteNode::GetPreviousID(int customer_id)
{
	Position position = positions[customer_id];
	if (position-- == served_customer.begin()) return DEPOT_ID;
	return *position;
}

int RouteNode::GetNextID(int customer_id)
{
	Position position = positions[customer_id];
	if (++position == served_customer.end()) return -1;
	return *position;
}

void RouteNode::Reverse(int customer_id_1, int customer_id_2)
{
	Position begin_position = positions[customer_id_1];
	Position end_position = positions[customer_id_2];
	for (; begin_position != end_position;) {
		std::swap(positions[*begin_position], positions[*end_position]);
		std::swap(*begin_position, *end_position);
		if (++begin_position == end_position) break;
		--end_position;
	}
}

CPosition begin(const RouteNode &route_node)
{
	return route_node.served_customer.cbegin();
}

CPosition end(const RouteNode &route_node)
{
	return route_node.served_customer.cend();
}

void Solution::Prepare()
{
	;
}

Solution &Solution::operator = (const Solution &solution)
{
	customer_nodes = solution.customer_nodes;
	route_nodes = solution.route_nodes;
	cost = solution.cost;
	return *this;
}

void Solution::Initialize(size_t customer_size, size_t route_size)
{
	cost = 0;
	clean(customer_nodes, customer_size);
	clean(route_nodes, route_size);
}

int Solution::GetUsedRouteCount()
{
	int ret = 0;
	for (size_t i = 0; i < route_nodes.size(); i++) {
		if (route_nodes[i].Empty()) continue;
		ret++;
	}
	return ret;
}

void Solution::Insert(int customer_id, int route_id, int next_id)
{
	customer_nodes[customer_id].Insert(route_id);
	route_nodes[route_id].Insert(customer_id, next_id);
}

int Solution::Erase(int customer_id, int route_id)
{
	customer_nodes[customer_id].Erase(route_id);
	return route_nodes[route_id].Erase(customer_id);
}

}
