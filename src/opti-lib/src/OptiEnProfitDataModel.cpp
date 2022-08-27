#include "OptiEnProfitDataModel.h"

#include "JsonReader.h"
#include "ConfigSol.h"
#include "TimeUtil.h"
#include "SolUtil.h"

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

#include <STD/String.hpp>

using namespace EnjoLib;

const char * OptiEnProfitDataModel::SOLAR_POS_FILE = "solar_pos.txt";

OptiEnProfitDataModel::OptiEnProfitDataModel(const ConfigSol & confSol, int horizonDays, int statingPoint)
: m_confSol(confSol)
, m_horizonHours(horizonDays * 24)
, m_statingPoint(statingPoint)
, m_currHour(TimeUtil().GetCurrentHour())
//, m_comps(JsonReader().ReadComputers())
, m_sys(JsonReader().ReadSystem())
, m_batPars(JsonReader().ReadBatteries().at(0)) /// TODO: extend
, m_habits(JsonReader().ReadHabits())
{
    const Str fname = m_confSol.m_outDir + "/" + SOLAR_POS_FILE;
    const Str powerData = SolUtil().GetLinesAsSingle(fname);
    m_power = VecD(powerData);
    const auto & comps = JsonReader().ReadComputers();
    AR2VEC(comps, m_comps);
}

OptiEnProfitDataModel::OptiEnProfitDataModel(const ConfigSol & confSol, std::vector<Habit> habits,
                                            const System & sys, const BatteryParams & batPars,
                                            int horizonDays, int statingPoint)
: m_confSol(confSol)
, m_horizonHours(horizonDays * 24)
, m_statingPoint(statingPoint)
, m_currHour(TimeUtil().GetCurrentHour())
//, m_comps(JsonReader().ReadComputers())
, m_sys(sys)
, m_batPars(batPars)
, m_habits(habits)
{
}

OptiEnProfitDataModel::~OptiEnProfitDataModel()
{
}

const EnjoLib::VecD & OptiEnProfitDataModel::GetPowerProductionData() const
{
    return m_power;
}

void OptiEnProfitDataModel::SetComputers(const std::vector<Computer> & computers)
{
    m_comps = computers;
}

void OptiEnProfitDataModel::SetPowerProduction(const EnjoLib::VecD & power)
{
    m_power = power;
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
    short binary[m_horizonHours];
    for (int i = 0; i < m_horizonHours; ++i)
    {
        binary[i] = 0;
    }

    EnjoLib::Matrix ret2;

    const int maxEl = 10e5;
    for (int i = 0; i < maxEl; ++i)
    {
        if (i % 10000 == 0)
        {
                LOGL << "created " << i << "/" << maxEl << Nl;
        }
        const int index = GMat().round(rmath.Rand(0, m_horizonHours-1));
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

double OptiEnProfitDataModel::GetHabitsUsage(int i) const
{
    if (i < m_habitsCache.size())
    {
        return m_habitsCache[i];
    }
    else
    {
        double sum = 0;
        const size_t szz = m_habits.size();
        for (int ih = 0; ih < szz; ++ih)
        {
            const Habit & hab = m_habits[ih];
            double usage = hab.watt_asleep;
            if (hab.IsOn(i))
            {
                usage = hab.watt;
            }
            sum += usage;
        }
        m_habitsCache.push_back(sum);
        return sum;
    }
}

bool OptiEnProfitDataModel::IsAnimateProgressBar() const
{
    const bool animateProgressBar = not GetConf().NO_PROGRESS_BAR;
    return animateProgressBar;
}
