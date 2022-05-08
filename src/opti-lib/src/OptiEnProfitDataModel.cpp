#include "OptiEnProfitDataModel.h"

#include "JsonReader.h"
#include "ConfigSol.h"

//#define BOOST_JSON_STACK_BUFFER_SIZE 1024
//#include <boost/json/src.hpp>
//#include <boost/json.hpp>

#include <Ios/Ifstream.hpp>
#include <Util/VecD.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/Tokenizer.hpp>
#include <Util/CharManipulations.hpp>
#include <Template/Array.hpp>
#include <Math/RandomMath.hpp>
#include <Math/GeneralMath.hpp>

using namespace EnjoLib;

OptiEnProfitDataModel::OptiEnProfitDataModel(const ConfigSol & confSol, int horizonDays, int statingPoint)
: m_confSol(confSol)
, m_horizonHours(horizonDays * 24)
, m_statingPoint(statingPoint)
, m_comps(JsonReader().ReadComputers())
, m_batPars(JsonReader().ReadBatteries().at(0))
{
    const char * fname = "/tmp/solar_pos.txt";
    {Ifstream solPosIn(fname);}
    const Tokenizer tok;
    const VecStr & lines = tok.GetLines(fname);
    for (const Str line : lines)
        m_power.Add(CharManipulations().ToDouble(line));
}

OptiEnProfitDataModel::~OptiEnProfitDataModel()
{
}

const EnjoLib::VecD & OptiEnProfitDataModel::GetPowerProductionData() const
{
    return m_power;
}

double OptiEnProfitDataModel::GetPowerProduction(int i) const
{
    return m_power.at(i + m_statingPoint);
}

template <int bits>
VecD ToBinary(int number)
{
    std::string binary = std::bitset<bits>(number).to_string(); //to binary
    //LOGL << "Binary = " << binary << Nl;
    VecD ret;
    CharManipulations cman;
    for (const char c : binary)
    {
        ret.Add(cman.ToDouble(Str(c)));
    }
    return ret;
}

EnjoLib::Matrix OptiEnProfitDataModel::GetData() const
{
    EnjoLib::Matrix ret;
    for (int dim = 0; dim < m_horizonHours; ++dim)
    {
        EnjoLib::VecD obs;
        for (double power = 0; power <= 1; power += 0.5)
        {
            obs.push_back(power);
        }
        ret.push_back(obs);
    }

    RandomMath rmath;
    rmath.RandSeed();
    short binary[m_horizonHours] {};

    EnjoLib::Matrix ret2;

    const int maxEl = 10e5;
    for (int i = 0; i < maxEl; ++i)
    {
        if (i % 10000 == 0)
        {
                LOGL << "created " << i << "/" << maxEl << Nl;
        }
        const int index = GMat().round(rmath.Rand(0, m_horizonHours-0.999));
        binary[index] = binary[index] == 0 ? 1 : 0;
        //LOGL << binary << Nl;

        VecD vec;
        CharManipulations cman;
        for (int b = 0; b < m_horizonHours; ++b)
        {
            vec.Add(binary[b]);
        }
        ret2.push_back(vec);
    }




    return ret2;
}
