#include "OptiEnProfitDataModel.h"

#include <Util/VecD.hpp>
#include <Util/CoutBuf.hpp>
#include <Template/Array.hpp>

using namespace EnjoLib;

OptiEnProfitDataModel::OptiEnProfitDataModel()
{
}

OptiEnProfitDataModel::~OptiEnProfitDataModel()
{
}

EnjoLib::Matrix OptiEnProfitDataModel::GetData() const
{
    EnjoLib::Matrix ret;
    for (int dim = 0; dim < 3; ++dim)
    {
        EnjoLib::VecD obs;
        for (int iobs = 0; iobs < 10; ++iobs)
        {
            obs.push_back(iobs);
        }
        ret.push_back(obs);
    }

    return ret;
}
