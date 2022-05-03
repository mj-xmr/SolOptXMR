#ifndef OptiSubjectEnProfit_H
#define OptiSubjectEnProfit_H

#include "IOptiSubject.h"
#include "StartEnd.h"

#include <STD/Vector.hpp>

class ISymbol;
class IPeriod;
class PredictorFactory;
enum class PredictorType;

class OptiEnProfitDataModel;
class OptiSubjectEnProfit : public EnjoLib::OptiMultiSubject // IOptiSubject
{
    public:
        OptiSubjectEnProfit(const OptiEnProfitDataModel & dataModel);
        virtual ~OptiSubjectEnProfit();

        double GetGoal() const;

        double Get(const double * in, int n) override;
        EnjoLib::VecD GetStart() const override;
        EnjoLib::VecD GetStep() const override;
        EnjoLib::Array<EnjoLib::OptiMultiSubject::Bounds> GetBounds() const override;

        //STDFWD::vector<OptiVarF> GetOptiVarsResult() override { return m_optiFloatResult; }

    protected:

    private:
        const OptiEnProfitDataModel & m_dataModel;

        std::vector<OptiVarF> m_optiFloat;
        std::vector<OptiVarF> m_optiFloatResult;

        //EnjoLib::VecD m_iterData;

        float m_sumMax = 0;
};

#endif // OptiSubjectEnProfit_H
