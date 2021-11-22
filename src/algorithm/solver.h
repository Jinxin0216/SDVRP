#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "core/basetypes.h"

namespace VRP
{
class Solver
{
protected:
	Solution solution;
	virtual void main_process() = 0;
	Clock main_clk;
public:
	virtual Solution Work();
};
}

#endif
