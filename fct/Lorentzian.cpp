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
    defaults.addParameter( "Baseline"     , 0.0, 0.01, 0.0, 0.33  );//par[0]
    defaults.addParameter( "Amplitude"     , 0.1, 0.01, -0.33, 0.33 );//par[1]
    defaults.addParameter( "Resonance" , 6, 0.01, 0, 10  );//par[2]
    defaults.addParameter( "FWHM"      , 1, 0.01, 0.0, 5 );//par[3]
  }

  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
    return par[0]
            + par[1]/(4.0*(x-par[2])*(x-par[2])/(par[3]*par[3]) + 1.0);
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: lorentzian.<br><br>"
            "f(x) = par[0] + par[1]/(4.0*(x-par[2])*(x-par[2])/(par[3]*par[3]) + 1.0)<br>"
            "where:<br>"
            "par0 = Baseline<br>"
            "par1 = Amplitude  <br>"
            "par2 = Resonance <br>"
            "par3 = FWHM <br>";
  }
}
