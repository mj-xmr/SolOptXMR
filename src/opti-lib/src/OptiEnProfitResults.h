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
        
        struct CommandsInfos
        {
            EnjoLib::Str commands;
            EnjoLib::Str infos;
        };

        EnjoLib::Str PrintScheduleComp(const Computer & comp, const EnjoLib::VecD & best) const;
        CommandsInfos PrintCommandsComp(const Computer & comp, const EnjoLib::VecD & best, int currHour, int maxDayCmdsLimit = -1) const;
        
        static const int SSH_TIMEOUT;
        
    protected:
    
    private:
        //const OptiEnProfitDataModel & m_dataModel;
};

#endif // OPTIMIZER_H
