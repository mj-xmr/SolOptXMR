#ifndef OptiEnProfitDataModel_H
#define OptiEnProfitDataModel_H

#include "Computer.h"
#include "BatteryParams.h"
#include "System.h"
#include "Habit.h"

#include <Statistical/Matrix.hpp>
#include <Util/VecD.hpp>
#include <Template/Array.hpp>

class ConfigSol;
class OptiEnProfitDataModel
{
    public:
        OptiEnProfitDataModel(const ConfigSol & confSol, int horizonDays = 4, int statingPoint = 0);
        virtual ~OptiEnProfitDataModel();

        EnjoLib::Matrix GetData() const;
        const EnjoLib::VecD & GetPowerProductionData() const;
        double GetPowerProduction(int i) const;
        //const VecD & powerProd = m_dataModel.GetPowerProduction();
        int GetHorizonHours() const { return m_horizonHours; }
        int GetStartingPoint() const { return m_statingPoint; }
        int GetCurrHour() const { return m_currHour; }

        const ConfigSol & GetConf() const { return m_confSol; }
        const EnjoLib::Array<Computer> & GetComputers() const { return m_comps; }
        const EnjoLib::Array<Habit> & GetHabits() const { return m_habits; }
        const BatteryParams & GetBatPars() const { return m_batPars; }
        const System & GetSystem() const { return m_sys; }

    protected:

    private:
        const ConfigSol & m_confSol;
        int m_horizonHours = 0;
        int m_statingPoint = 0;
        int m_currHour = 0;
        EnjoLib::VecD m_power;
        EnjoLib::Array<Computer> m_comps;
        EnjoLib::Array<Habit> m_habits;
        System m_sys;
        BatteryParams m_batPars;
};

#endif // OptiEnProfitDataModel_H
