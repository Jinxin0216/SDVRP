#ifndef __HELPER_H__
#define __HELPER_H__

#include <random>
#include <chrono>
#include <vector>
#include <iostream>

using std::vector;

#define ECHO(x) std::cout << #x << ' ' << x << std::endl

namespace VRP {

const double eps = 1e-3;
inline int dcmp(double x) { return (x > eps) - (x < -eps); }

inline bool random_test(double p)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> dis(0, 1);
	return dis(gen) < p;
}

inline int random_generate(int l, int r)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(l, r);
	return dis(gen);
}

inline vector<int> gen_range_numbers(size_t size)
{
	vector<int> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(i);
	}
	return vec;
}

class Clock {
private:
	std::chrono::time_point<std::chrono::steady_clock> begin_time_point;
public:
	Clock() {
		begin_time_point = std::chrono::steady_clock::now();
	}
	int GetDuration() {
		auto current_time_point = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::seconds>(current_time_point - begin_time_point).count();
	}
	int GetHighPricisionDuration() {
		auto current_time_point = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(current_time_point - begin_time_point).count();
	}
};

template <typename T>
void fix(vector<vector<T>> &table, size_t row, size_t col, T default_value = T())
{
	size_t old_row = table.size();
	if (old_row == 0) {
		table.resize(row, vector<T>(col, default_value));
		return;
	}
	size_t old_col = table[0].size();
	if (old_row < row) {
		table.resize(row);
		for (size_t i = old_row; i < row; i++) {
			table[i] = vector<T>(old_col, default_value);
		}
	}
	if (old_col < col) {
		for (size_t i = 0; i < row; i++) {
			table[i].resize(col, default_value);
		}
	}
}

template <typename T>
void clean(vector<vector<T>> &table, size_t row, size_t col, T default_value = T())
{
	table.resize(row);
	for (T &one_row: table) {
		one_row.resize(col);
		for (T &x: one_row) {
			x = default_value;
		}
	}
}

template <typename T>
void fix(vector<T> &table, size_t size, T default_value = T())
{
	size_t old_sz = table.size();
	if (old_sz < size) {
		table.resize(size, default_value);
	}
}

template <typename T>
void clean(vector<T> &table, size_t size, T default_value = T())
{
	table.resize(size);
	for (T &x: table) x = default_value;
}

// template <typename CustomerType, typename RouteType, typename T, typename... Targs>
// void sync(vector<CustomerType> &customers, vector<RouteType> &routes, bool change_flag, vector<vector<T>> &table, Targs... TBargs)
// {
// 	if (!change_flag) {
// 		for (auto &route: routes) {
// 			if (route.Empty()) {
// 				change_flag = true;
// 			}
// 		}
// 		if (!change_flag) return;
// 	}
// 	vector<int> idx;
// 	for (size_t i = 0, j = 0; i < routes.size(); i++) {
// 		idx.push_back(j);
// 		if (!route.Empty()) j++;
// 	}
// 	sync(customers, routes, true, TBargs);
// }
// 
// 	template <typename CustomerType, typename RouteType, typename T>
// void sync(vector<CustomerType> &customers, vector<RouteType> &routes, bool change_flag, vector<vector<T>> &table)
// {
// 	change_flag = false;
// 	for (auto &route: routes) {
// 		if (route.Empty()) {
// 			change_flag = true;
// 		}
// 	}
// 	if (!change_flag) return;
// }

}

#endif
