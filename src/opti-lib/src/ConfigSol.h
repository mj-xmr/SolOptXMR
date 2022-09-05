#ifndef CONFIGSOL_H
#define CONFIGSOL_H

#include "ConfigBase.h"
#include <Util/Str.hpp>

enum class PredictorType;
enum class PriceType;

class ConfigSol : public ConfigBase
{
    public:
        ConfigSol();
        virtual ~ConfigSol();

        void RegisterAndReadBools (EnjoLib::Istream & ifs) override;
        void RegisterAndReadInts  (EnjoLib::Istream & ifs) override;
        void RegisterAndReadFloats(EnjoLib::Istream & ifs) override;
        void RegisterAndReadStrs  (EnjoLib::Istream & ifs) override;

        void FromOptsNumeric(const OptionsNumeric & optsNum) override;

        void UpdateFromOther(const ConfigSol & cfgTSCmdLine);

        bool POWEROFF = false;     constexpr static const char * DESCR_POWEROFF = "Poweroff all computers rather than suspending them.";
        bool NO_PROGRESS_BAR = false; constexpr static const char * DESCR_NO_PROGRESS_BAR = "Disable progress bar";
        bool NO_NEW_SOLUTIONS = false; constexpr static const char * DESCR_NO_NEW_SOLUTIONS = "Disable solutions progress";
        bool NO_GNUPLOT = false;      constexpr static const char * DESCR_NO_GNUPLOT = "Disable gnuplot";
        bool NO_SCHEDULE = false;     constexpr static const char * DESCR_NO_SCHEDULE = "Disable computer schedule";
        bool NO_COMPUTERS = false;     constexpr static const char * DESCR_NO_COMPUTERS = "Don't simulate mining on computers";

        long int DAYS_HORIZON = 3; constexpr static const char * DESCR_DAYS_HORIZON = "How many days ahead you'd like to predict";
        long int DAYS_START = 0;   constexpr static const char * DESCR_DAYS_START   = "The starting day";
        long int RANDOM_SEED = 0;  constexpr static const char * DESCR_RANDOM_SEED  = "Random seed. Use any integer number to 'fixate' the random solutions";
        long int DAYS_LIMIT_COMMANDS = 1; constexpr static const char * DESCR_DAYS_LIMIT_COMMANDS  = "Generate commands up to which day? UNSTABLE"; /// TODO: proper "at"
        long int NUM_SOLUTIONS = 3; constexpr static const char * DESCR_NUM_SOLUTIONS  = "The number of alternative solutions to show";

        //long int SYSTEM_VOLTAGE = 12;   constexpr static const char * DESCR_SYSTEM_VOLTAGE   = "System voltage: 12 / 24";
        float BATTERY_CHARGE = 0;  constexpr static const char * DESCR_BATTERY_CHARGE = "Initial battery charge in Ah";
        float BATTERY_CHARGE_MAX_PERCENTAGE = 0;  constexpr static const char * DESCR_BATTERY_CHARGE_MAX_PERCENTAGE = "Max percentage to which the battery can be charged";
        float HASHRATE_BONUS = 0;  constexpr static const char * DESCR_HASHRATE_BONUS = "Hashrate bonus for the current hour";
        float MAX_RAW_SOLAR_INPUT = 0;  constexpr static const char * DESCR_RAW_SOLAR_INPUT = "The maximum wattage, that your panels can produce today (excluding weather conditions)";

        //EnjoLib::Str SYSTEM_TYPE = "island"; constexpr static const char * DESCR_SYSTEM_TYPE = "System type: island / gen+buy / gen+buy+sell / gen+sell";
        EnjoLib::Str m_outDir = "/tmp/";  constexpr static const char * DESCR_OUT_DIR = "Output directory";
        
        EnjoLib::Str m_ignoreComputers = ""; constexpr static const char * DESCR_IGNORE_COMPUTERS  = "Ignore these computers (comma separated)";
        EnjoLib::Str m_onlyComputers = "";   constexpr static const char * DESCR_ONLY_COMPUTERS  = "Use only these computers (comma separated)";


    protected:
        EnjoLib::Str GetFileNameProt() const override;

    private:
};

#endif // CONFIGTS_H
