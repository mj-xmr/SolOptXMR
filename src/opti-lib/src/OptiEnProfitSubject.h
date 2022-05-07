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
enum class PredictorType;


struct BatterySimulation
{
    double MAX_USAGE = 11; /// TODO: Rename to discharge
	double MUL_POWER_2_CAPACITY = 0.1;
			static constexpr double T_DELTA_HOURS = 1;
				double MAX_CAPACITY = 60;
        double MIN_LOAD = MAX_CAPACITY / 2; //# Assuming a lead-acid
        //double load = MIN_LOAD * 0.95;
        double load = MIN_LOAD * 1.1;
        double DISCHARGE_PER_HOUR = 0.1;
        double initial_load = true;
        double num_undervolted = 0;
        double num_undervolted_initial = 0;
        double num_overvolted = 0;
        double num_overused = 0;

    double iter_get_load(double inp, double out, double hours=T_DELTA_HOURS);
};


class OptiEnProfitDataModel;
class OptiSubjectEnProfit : public EnjoLib::OptiMultiSubject // IOptiSubject
{
    public:
        OptiSubjectEnProfit(const OptiEnProfitDataModel & dataModel);
        virtual ~OptiSubjectEnProfit();

        double GetGoal() const;

        double Get(const double * in, int n) override;
        double GetVerbose(const EnjoLib::Matrix & dataMat, bool verbose =false);
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

        SimResult Simulate(int i, const EnjoLib::Matrix & dataMat, double bonusMul) const;


    protected:

    private:
        const OptiEnProfitDataModel & m_dataModel;

        std::vector<OptiVarF> m_optiFloat;
        std::vector<OptiVarF> m_optiFloatResult;

        //EnjoLib::VecD m_iterData;
        float m_sumMax = 0;
};

#endif // OptiSubjectEnProfit_H
