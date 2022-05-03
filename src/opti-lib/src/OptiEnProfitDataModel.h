#ifndef OptiEnProfitDataModel_H
#define OptiEnProfitDataModel_H

#include <Statistical/Matrix.hpp>
#include <Template/ArrayFwd.hpp>

class OptiEnProfitDataModel
{
    public:
        OptiEnProfitDataModel();
        virtual ~OptiEnProfitDataModel();

        EnjoLib::Matrix GetData() const;

    protected:

    private:
};

#endif // OptiEnProfitDataModel_H
