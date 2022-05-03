#ifndef DISTRIBSTEST_H
#define DISTRIBSTEST_H

namespace EnjoLib
{
    class StdDev;
}

class DistribsTest
{
    public:
        DistribsTest();
        virtual ~DistribsTest(){}

        double NormalPDF(double mean, const EnjoLib::StdDev & stdDev, double arg) const;

    protected:

    private:
};

#endif // DISTRIBS_H
