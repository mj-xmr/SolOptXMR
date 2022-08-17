#ifndef OPTIMIZERENPROFITRES_H
#define OPTIMIZERENPROFITRES_H

#include <Util/Str.hpp>
#include <Util/VecD.hpp>
#include <Statistical/Matrix.hpp>

class Computer;
class OptiEnProfitDataModel;

class OptiEnProfitResults
{
    public:
        OptiEnProfitResults();
        virtual ~OptiEnProfitResults();

        EnjoLib::Str PrintScheduleComp(const Computer & comp, const EnjoLib::VecD & best, int currHour) const;
        
    protected:
    
    private:
        //const OptiEnProfitDataModel & m_dataModel;
};

#endif // OPTIMIZER_H
