#ifndef OPTIMIZERENPROFITRES_H
#define OPTIMIZERENPROFITRES_H

#include <Util/Str.hpp>
#include <Util/VecD.hpp>
#include <Statistical/Matrix.hpp>

class Computer;
class OptiEnProfitDataModel;
class Sol0Penality;

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

        EnjoLib::Str PrintOptiProgression(const EnjoLib::VecD & goals, int horizonHours) const;
        EnjoLib::Str PrintMultipleSolutions(const OptiEnProfitDataModel & dataModel, const STDFWD::vector<Sol0Penality> & solutions0Penality, int maxSolutions) const;
        EnjoLib::Str PrintSolution(const OptiEnProfitDataModel & dataModel, const EnjoLib::Matrix & bestMat, double maxHashes = 0) const;
        EnjoLib::Str PrintScheduleCompGraph(const Computer & comp, const EnjoLib::VecD & best) const;
        CommandsInfos PrintCommandsComp(const Computer & comp, const EnjoLib::VecD & best, int currHour, int maxDayCmdsLimit = 1) const;

        static const int SSH_TIMEOUT_S;

    protected:

    private:
        //const OptiEnProfitDataModel & m_dataModel;
};

#endif // OPTIMIZER_H
