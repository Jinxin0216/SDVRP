/**
 *   Copyright (C) 2017 All rights reserved.
 *   
 *   filename: main.cpp
 *   author: doublehh
 *   e-mail: sserdoublehh@foxmail.com
 *   create time: 2017-02-09 21:56:18
 *   last modified: 2017-02-09 21:56:18
 */
#include "core/utility.h"
#include "core/instance.h"
#include "algorithm/local_search.h"
#include "algorithm/tabu_search.h"

using namespace VRP;

int main(int argc, char** argv)
{
	srand(time(NULL));
	Solver* solver = new VRP::TabuSearch;
	for (int i = 1; i < argc; i++) {
		puts(argv[i]);
		InitInstance<SDVRP>(argv[i]);
		solver->Work();
	}
}
