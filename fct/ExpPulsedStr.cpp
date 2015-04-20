/********************************CCP headers ***********************************/
#include <iostream>                             //Header file for outputting data
#include <fstream>                              //Header file for outputting data
#include <math.h>                               //Header file for math operations
#include <stdio.h>
#include <vector>
#include <sstream>   
#include <string.h>
#include <iostream>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
using namespace std;

//The header file where the functions below are constructed.
#include "parameters.h"

//Wrap in "C" for the compiler.
extern "C" 
{ 
//Write the description here. Not critical.
void description(string &text)
{
    text = "Name of Function: PulsedStrExp.<br><br>"
           "f(x) = Read pulsed beam document."
           "par0 = Amplitude<br>"
           "par1 = Rate  <br>"
           "par2 = Beta  <br>";
}

//The defalt initial parameters loaded to the table.
void defaultParameters(Parameters &defaults)
{
    defaults.addParameter( "Amplitude" , 0.1, 0.001, -0.33, 0.33 );//par[0]
    defaults.addParameter( "Rate" , 0.1, 0.001, 0.0, 1.0 );//par[1]
    defaults.addParameter( "Beta"      , 1.0, 0.001, 0.0,  2.0 );//par[2]
}

double Delta = 4.0;
double Tau   = 1.2096;
int nb_lines = 1;//you can change this to 2, 3,4, but you must change the defaultparameters
double epsabs = 1e-4;
double epsrel = 1e-4;

//-----------------------------------T1 Parameters------------------------------------
//Calculates f(1/T1) knowing Tau
double taup(double it1)
{ 
    return 1.0/(1.0/Tau +   it1);
}  
//Needed for pulsed beam
double pstep(double it1,double t)
{
    return (taup(it1)/Tau)*(1.0-exp(-t/taup(it1)))/(1.0-exp(-t/Tau));
}
//Needed for single counters
double sstep(double Tau,double t)
{
    return Tau*(1.0-exp(-t/Tau));
}

//defenition INTEGRAL OVER STR EXP---------------------------------------------------------
double LAM,BETA;
double f (double tp, void * params) 
{
    double t = *(double *) params;
    double f = exp(-(t-tp)/Tau)*exp(-pow((t-tp)*LAM,BETA));
    return f;
}
//gsl library execution of the integral
double StrInt(double lambda,double beta,double t0,double tf,double tc)
{
    LAM = lambda;
    BETA = beta;
    double result = 0.0, error = 0.0, tcst = tc;
    gsl_integration_workspace *w = gsl_integration_workspace_alloc (1000);
    gsl_function F;
    F.function = &f;
    F.params = &tcst;
    gsl_set_error_handler_off ();//turning off native error messages which might crash the
    //program
    //status is either 0 (success) or -1
    int status = gsl_integration_qag (&F, t0, tf, epsabs,epsrel, 1000,1,w, &result, &error);
    if(status != 0 )
    {
        result = 0.0;
        cout<<"\nError: PulsedSqrExp.cpp\nThe integral has NOT converged!\n";
    }
    gsl_integration_workspace_free (w);

    return result;
}

//This must the same number of parameters as described in the default parameters above.
double function(double x, const std::vector<double> par)
{
    double fun   = 0;
    if ( x  > 0. && x <= Delta )
        for ( int k = 0; k < nb_lines ; k++ )
            fun +=  (par.at(k)/(Tau*(1.0-exp(-x/Tau))))
                    *StrInt(par.at(k+nb_lines),par.at(k+2*nb_lines),0.0,x,x);
    if ( x > Delta )
        for ( int k = 0; k < nb_lines ; k++ )
            fun +=  (par.at(k)/(Tau*(1.0-exp(-Delta/Tau))*exp(-(x-Delta)/Tau)))
                    *StrInt(par.at(k+nb_lines),par.at(k+2*nb_lines),0.0,Delta,x);
    return fun;
}

}
