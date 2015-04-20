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
    defaults.addParameter( "Amplitude1"     , 0.1, 0.01, -0.33, 0.33 );//par[1]
    defaults.addParameter( "Resonance1" , 6, 0.01, 0, 12  );//par[2]
    defaults.addParameter( "FWHM1"      , 1, 0.01, 0.0, 5 );//par[3]
    defaults.addParameter( "Amplitude2"     , 0.1, 0.01, -0.33, 0.33 );//par[4]
    defaults.addParameter( "Resonance2" , 5, 0.01,0, 12  );//par[5]
    defaults.addParameter( "FWHM2"      , 1, 0.01, 0.0, 5 );//par[6]
  }

  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
    return par[0]
            + par[1]*exp( -(x-par[2])*(x-par[2])/(2.0*par[3]*par[3]/(2.355*2.355) ))
            + par[4]*exp( -(x-par[5])*(x-par[5])/(2.0*par[6]*par[6]/(2.355*2.355) ));
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: two gaussians.<br><br>"
            "f(x) = par[0] + par[1]*exp( -(x-par[2])*(x-par[2])/(2.0*par[3]*par[3]/(2.355*2.355) ))<br>"
            "+ par[4]*exp( -(x-par[5])*(x-par[5])/(2.0*par[6]*par[6]/(2.355*2.355) ))<br>"
            "where:<br>"
            "par0 = Baseline<br>"
            "par1 = Amplitude1  <br>"
            "par2 = Resonance1 <br>"
            "par3 = FWHM1 <br>"
            "par4 = Amplitude2  <br>"
            "par5 = Resonance2 <br>"
            "par6 = FWHM2 <br>";
  }
}
