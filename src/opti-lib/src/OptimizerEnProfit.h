#ifndef OPTIMIZERENPROFIT_H
#define OPTIMIZERENPROFIT_H

#include "OptimizerBase.h"
#include "StartEnd.h"

#include <Util/VecD.hpp>
#include <Statistical/Matrix.hpp>

class ISymbol;
class IOptimizable;
class IPeriod;
class ProfitsCalc;
class StartEnd;
class TSFunFactory;
template<class T> class OptiVar;

class OptiEnProfitDataModel;

class OptimizerEnProfit : public EnjoLib::IMultiDimIterConsumerTpl //OptimizerBase
{
    public:
        OptimizerEnProfit(const OptiEnProfitDataModel & dataModel);
        virtual ~OptimizerEnProfit();

        void operator()();

        void Consume(const EnjoLib::VecD & data) override; // IMultiDimIterConsumerTpl

        bool Consume2(const EnjoLib::Matrix & data, bool needNewline);
        void AddSpace(const EnjoLib::VecD & data);
        void RecalcComputationCosts();

        bool IsUseHash() const;
        const EnjoLib::VecD & GetGoals() const { return m_goals; }
        double GetGoal() const { return m_goal; }

        using BigInt = unsigned long long; // int is way too small.

        const int static HOURS_IN_DAY;
        const BigInt static MAX_NUM_COMBINATIONS;
        const double static MAX_FAILED_COMBINATIONS;
        const double static MIN_POS_2_NEG_CHANGE_RATIO;
        const double static GOAL_INITIAL;

    protected:
        void RandomSearch();
        void PrintSolution(const EnjoLib::Matrix & best) const;
        //STDFWD::vector<const IPeriod *> GetPeriods() const override;
        //void PrintStats() const override;
        //void PrintStatsSummary() const override;
    private:
        EnjoLib::Str GetT() const;

        const OptiEnProfitDataModel & m_dataModel;
        EnjoLib::Matrix m_data;

        double m_goal = GOAL_INITIAL;
        int m_uniqueSolutions = 0;
        int m_uniqueSolutionsPrev = 0;
        EnjoLib::VecD m_goals;
        int m_numFailed = 0;
        double m_relPos2Neg = 0;
        double m_relChangePositive = 0;
        double m_relChangeNegative = 0;
};

#endif // OPTIMIZER_H
