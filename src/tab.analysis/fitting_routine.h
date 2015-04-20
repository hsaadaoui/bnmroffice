#ifndef  fitting_routine_H
#define  fitting_routine_H

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnSimplex.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MinosError.h"
#include "Minuit2/FCNBase.h"
#include "plugin_read.h"
using namespace ROOT::Minuit2;



/********************************MODEL      ********************************/
//This calculates the chisq and send it to Minuit, with the virtual double operator()
//Do not change unless you know what you are doing.
class MODEL : public FCNBase
{
public:
    MODEL(function &fct,
          QVector< QVector< QVector<double> > > xdata,
          QVector< QVector< QVector<double> > > ydata):
        Fct(fct),xData(xdata),yData(ydata),ErrorDef(1.){}

    ~MODEL() {}

    virtual double Up() const {return ErrorDef;}

    virtual double operator()(const std::vector<double>&) const;

    double Chisquared(QVector<QVector<double> >,QVector<QVector<double> >,
                      vector<double>) const;

    QVector<double> error(QVector<QVector<double> >,QVector<QVector<double> >,vector<double>)const;

    std::vector<double> getParList(int f,const std::vector<double>&pars) const;

    void setErrorDef(double def) {ErrorDef = def;}

    void setNbPars(int pt) {pT = pt;}

    void setNbSharedPars(QVector<int> shared) {ParShare = shared;}

    void setNbFiles(int nbf) {nbFiles = nbf;}

    void setNbParsFct(int pf) {pF = pf;}

private:
    QVector<QVector<QVector<double> > > xData;

    QVector<QVector<QVector<double> > > yData;

    QVector<int>    ParShare;

    short    nbFiles;

    short    pT;

    short    pF;

    double ErrorDef;

    function& Fct;
};

/********************************Performs FIT      ********************************/
//Do not change unless you know what you are doing.
class FIT
{
public:
    FIT(function &fct,QString min,QString err, bool type,bool loop,
        QVector< QVector< QVector<double> > > X,
        QVector< QVector< QVector<double> > > Y):
        Fct(fct),Minimizer(min),Error(err),Global(type),Loop(loop),
        xData(X),yData(Y){}

    ~FIT() {}

    void PARAMETERS(QVector<double> &parValue,
                    QVector<double> &parErrNeg,QVector<double> &parErrPos,
                    QVector<double> &chisq,
                    QVector<double> parMin,QVector<double> parMax,
                    QVector<int> parFix, QVector<int> parShare);

    bool START(MnUserParameters upar, function Fct,
              QVector<int> Share,QVector<int> Fix,
              QVector<QVector<QVector<double> > > X,
              QVector<QVector<QVector<double> > > Y,
              QVector<double> &Value,
              QVector<double> &ErrNeg,
              QVector<double> &ErrPos,
              double &Chisq);

    MnUserParameters userParameters(int,QVector<QString>,QVector<int>,QVector<int>,
                                    QVector<double>,QVector<double>,
                                    QVector<double>,QVector<double>);

private:

    QVector<QVector<QVector<double> > > xData;

    QVector<QVector<QVector<double> > > yData;

    bool     Global;

    bool     Loop;

    QString  Minimizer;

    QString  Error;

    function& Fct;
};

#endif
