#ifndef OptiSubjectEnProfit_H
#define OptiSubjectEnProfit_H

#include "IOptiSubject.h"
#include "StartEnd.h"
#include <Util/VecD.hpp>
#include <Statistical/Matrix.hpp>

#include <STD/Vector.hpp>

class ISymbol;
class IPeriod;
class PredictorFactory;
class System;
struct Solution;
enum class PredictorType;

class OptiEnProfitDataModel;
class OptiSubjectEnProfit : public EnjoLib::OptiMultiSubject // IOptiSubject
{
public:
    OptiSubjectEnProfit(const OptiEnProfitDataModel & dataModel);
    virtual ~OptiSubjectEnProfit();

    double GetGoal() const;
    double GetPenality() const { return m_penalitySum; }

    double Get(const double * in, int n) override;
    Solution GetVerbose(const EnjoLib::Matrix & dataMat, bool verbose =false, double maxHashes = 0);
    EnjoLib::VecD GetStart() const override;
    EnjoLib::VecD GetStep() const override;
    EnjoLib::Array<EnjoLib::OptiMultiSubject::Bounds> GetBounds() const override;

    //STDFWD::vector<OptiVarF> GetOptiVarsResult() override { return m_optiFloatResult; }
    void UpdateOutput();
    EnjoLib::VecD m_hashes, m_loads, m_penalityUnder, m_input, m_prod, m_hashrateBonus, m_usages;

    double HashrateBonus(int hour) const;
    double HashrateBonusNonCached(int hour) const;
    void OutputVar(const EnjoLib::VecD & data, const EnjoLib::Str & descr, bool plot = true) const;


    struct SimResult
    {
        double sumHashes = 0;
        double sumPowerUsage = 0;

        void Add(const SimResult & other)
        {
            sumHashes += other.sumHashes;
            sumPowerUsage += other.sumPowerUsage;
        }
    };

    SimResult Simulate(int i, int currHour, size_t compSize, const EnjoLib::Matrix & dataMat, double bonusMul, double bonusMulMA, bool isInitialLoad) const;


protected:

private:
    const OptiEnProfitDataModel & m_dataModel;

    std::vector<OptiVarF> m_optiFloat;
    std::vector<OptiVarF> m_optiFloatResult;

    //EnjoLib::VecD m_iterData;
    double m_sumMax = 0;
    int m_currHour = 0;
    double m_penalitySum = 0;
};

#endif // OptiSubjectEnProfit_H
