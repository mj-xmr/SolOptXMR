#ifndef OPTIMIZERENPROFIT_H
#define OPTIMIZERENPROFIT_H

#include "OptimizerBase.h"
#include "StartEnd.h"

#include <Util/Str.hpp>
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

        void AddSpace(const EnjoLib::VecD & data);

    protected:
        //STDFWD::vector<const IPeriod *> GetPeriods() const override;
        //void PrintStats() const override;
        //void PrintStatsSummary() const override;
    private:
        const OptiEnProfitDataModel & m_dataModel;
        EnjoLib::Matrix m_data;
};

#endif // OPTIMIZER_H
