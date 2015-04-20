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
    defaults.addParameter( "Amplitude1" , 0.2, 0.001, 0.0, 1.0 );//par[0]
    defaults.addParameter( "Rate1"      , 0.1, 0.001, 0.0,  1.0 );//par[1]
    defaults.addParameter( "Amplitude2" , 0.2, 0.001, 0.0, 1.0 );//par[2]
    defaults.addParameter( "Rate2"      , 0.1, 0.001, 0.0,  1.0 );//par[3]
  }

  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
    return par[0] *exp( - par[1]*x)+ par[2]*exp( - par[3]*x);
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: exponential.<br><br>"
            "f(x) = par[0] *exp( - par[1]*x)+ par[2]*exp( - par[3]*x)<br><br>"
            "where:<br>"
            "par0 = Amplitude1<br>"
            "par1 = Rate1  <br>"
           "par2 = Amplitude2<br>"
           "par3 = Rate2  <br>";
  }
}
