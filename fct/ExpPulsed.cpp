/********************************CCP headers ***********************************/
#include <iostream>                             //Header file for outputting data
#include <fstream>                              //Header file for outputting data
#include <math.h>                               //Header file for math operations
#include <stdio.h>
#include <vector>
#include <sstream>   
#include <string.h>
#include <iostream>
using namespace std;

//The header file where the functions below are constructed.
#include "parameters.h"

//Wrap in "C" for the compiler.
extern "C" 
{ 
  //The defalt initial parameters loaded to the table.
  void defaultParameters(Parameters &defaults)
  {
    defaults.addParameter( "Amplitude" , 0.15, 0.01, 0.0, 0.330  );//par[0]
    defaults.addParameter( "Rate"      , 0.05, 0.01, 0.0, 1.0 );//par[1]
  }

 double Delta = 4.0;
 double Tau   = 1.2096;
 int nb_lines = 1;
 double fun   = 0;

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


  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
   fun = 0.0;
   if ( x  > 0. && x <= Delta )
      for ( int k = 0; k < nb_lines ; k++ )
        fun = par.at(k)*pstep(par.at(k+nb_lines),x);
    if ( x > Delta )  
      for ( int k = 0; k < nb_lines ; k++ )
        fun = par.at(k)*pstep(par.at(k+nb_lines),Delta)*exp(-(x-Delta)*par.at(k+nb_lines));
    return fun;
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: exponential.<br><br>"
            "f(x) = par[0]*exp( -par[1]*x )<br><br>"
            "where:<br>"
            "par0 = Amplitude<br>"
            "par1 = Rate  <br>";
  }
}
