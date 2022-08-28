#ifndef __OPTITESTUTIL_H
#define __OPTITESTUTIL_H

#include <Util/Str.hpp>
#include <Util/VecFwd.hpp>

class Computer;
class OptimizerEnProfit;

class OptiTestUtil
{
public:
	OptiTestUtil();
	virtual ~OptiTestUtil();

	static EnjoLib::Str compSched_macAddr;
    static EnjoLib::Str compSched_hostname;

    Computer GetCompTestSched() const;
    OptimizerEnProfit TestEdgeSolGetOptimizer(const EnjoLib::VecD & genPower, int horizon, int startingPoint, int num_computers = 1) const;

protected:

private:

};

#endif // __OPTITESTUTIL_H
