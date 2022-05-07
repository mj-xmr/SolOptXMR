#ifndef OptiEnProfitDataModel_H
#define OptiEnProfitDataModel_H

#include "Computer.h"

#include <Statistical/Matrix.hpp>
#include <Util/VecD.hpp>
#include <Template/Array.hpp>

class OptiEnProfitDataModel
{
    public:
        OptiEnProfitDataModel(int horizonDays = 4, int statingPoint = 0);
        virtual ~OptiEnProfitDataModel();

        EnjoLib::Matrix GetData() const;
        const EnjoLib::VecD & GetPowerProductionData() const;
        double GetPowerProduction(int i) const;
        //const VecD & powerProd = m_dataModel.GetPowerProduction();
        int GetHorizonHours() const { return m_horizonHours; }
        int GetStartingPoint() const { return m_statingPoint; }

        const EnjoLib::Array<Computer> & GetComputers() const { return m_comps; }

    protected:

    private:
        int m_horizonHours = 0;
        int m_statingPoint = 0;
        EnjoLib::VecD m_power;
        EnjoLib::Array<Computer> m_comps;
};

#endif // OptiEnProfitDataModel_H
