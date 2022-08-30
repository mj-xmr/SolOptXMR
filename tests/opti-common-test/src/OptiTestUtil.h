#ifndef __OPTITESTUTIL_H
#define __OPTITESTUTIL_H


#include <Util/Str.hpp>
#include <Util/VecD.hpp>
#include <Template/BuilderTpl.hpp>

#include <initializer_list>

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

protected:

private:
};

class OptiTestUtilConf
{
public:
    enum class Pars
    {
        NO_PROGRESS_BAR,
        NO_GNUPLOT,
        NO_SCHEDULE,
        NO_NEW_SOLUTIONS,
        BATTERY_CHARGE,
        NUM_SOLUTIONS,
        EXPECT_EMPTY_SCHEDULE,
        END
    };

    using BuilderT = EnjoLib::BuilderTpl<OptiTestUtilConf, Pars, double>;
    static BuilderT Build() { return BuilderT(); }

	OptiTestUtilConf(const BuilderT & builder);
	virtual ~OptiTestUtilConf();

	static EnjoLib::Str compSched_macAddr;
    static EnjoLib::Str compSched_hostname;


    OptimizerEnProfit TestEdgeSolGetOptimizer(const EnjoLib::Str & name, const EnjoLib::VecD & genPower,
                                              int horizon, int startingPoint,
                                              const EnjoLib::VecD & compHashMultpliers = {1}) const;

    void Add(const Pars & key, double val);
    double Get(const Pars & key) const;

protected:
    OptiTestUtilConf();

private:
    EnjoLib::VecD m_pars;

};


#endif // __OPTITESTUTIL_H
