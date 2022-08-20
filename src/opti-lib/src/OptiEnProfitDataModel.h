#ifndef OptiEnProfitDataModel_H
#define OptiEnProfitDataModel_H

#include "Computer.h"
#include "BatteryParams.h"
#include "System.h"
#include "Habit.h"

#include <Statistical/Matrix.hpp>
#include <Util/VecD.hpp>
#include <Template/Array.hpp>

#include <STD/Vector.hpp>

class ConfigSol;
class OptiEnProfitDataModel
{
    public:
        OptiEnProfitDataModel(const ConfigSol & confSol, int horizonDays = 4, int statingPoint = 0);
        virtual ~OptiEnProfitDataModel();

        EnjoLib::Matrix GetData() const;
        const EnjoLib::VecD & GetPowerProductionData() const;
        inline double GetPowerProduction(int i) const
        {
            return m_power.at(i + m_statingPoint);
        }
        //const VecD & powerProd = m_dataModel.GetPowerProduction();
        int GetHorizonHours() const { return m_horizonHours; }
        int GetStartingPoint() const { return m_statingPoint; }
        int GetCurrHour() const { return m_currHour; }

        const ConfigSol & GetConf() const { return m_confSol; }
        //const EnjoLib::Array<Computer> & GetComputers() const { return m_comps; }
        const std::vector<Computer> & GetComputers() const { return m_comps; }
        const EnjoLib::Array<Habit> & GetHabits() const { return m_habits; }
        double GetHabitsUsage(int i) const;
        const BatteryParams & GetBatPars() const { return m_batPars; }
        const System & GetSystem() const { return m_sys; }
        bool IsAnimateProgressBar() const;

    protected:

    private:
        const ConfigSol & m_confSol;
        int m_horizonHours = 0;
        int m_statingPoint = 0;
        int m_currHour = 0;
        EnjoLib::VecD m_power;
        //mutable EnjoLib::VecD m_habitsCache;
        mutable std::vector<double> m_habitsCache;
        //EnjoLib::Array<Computer> m_comps;
        std::vector<Computer> m_comps; // Choosing std::vector for runtime speed reasons.
        EnjoLib::Array<Habit> m_habits;
        
        System m_sys;
        BatteryParams m_batPars;
};

#endif // OptiEnProfitDataModel_H
