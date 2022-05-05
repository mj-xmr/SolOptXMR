#ifndef OptiEnProfitDataModel_H
#define OptiEnProfitDataModel_H

#include <Statistical/Matrix.hpp>
#include <Util/VecD.hpp>
#include <Template/ArrayFwd.hpp>

class OptiEnProfitDataModel
{
    public:
        OptiEnProfitDataModel(int horizonDays = 4, int statingPoint = 0);
        virtual ~OptiEnProfitDataModel();

        EnjoLib::Matrix GetData() const;
        const EnjoLib::VecD & GetPowerProduction() const;
        int GetHorizonHours() const { return m_horizonHours; }
        int GetStartingPoint() const { return m_statingPoint; }

    protected:

    private:
        int m_horizonHours = 0;
        int m_statingPoint = 0;
        EnjoLib::VecD m_power;
};

#endif // OptiEnProfitDataModel_H
