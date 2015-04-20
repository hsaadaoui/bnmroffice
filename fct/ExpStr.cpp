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
    defaults.addParameter( "Amplitude" , 0.2, 0.01, 0.0, 1.0 );//par[0]
    defaults.addParameter( "Rate"      , 0.1, 0.01, 0.0, .2 );//par[1]
    defaults.addParameter( "Power"     , 0.8, 0.01, 0.5, 1.5  );//par[2]
  }

  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
    return par[0]*exp( -pow( par[1]*x, par[2] ) );
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: streched exponential.<br><br>"
            "f(x) = par[0]*exp( -pow( par[1]*x, par[2] ))<br><br>"
            "where:<br>"
            "par0 = Amplitude<br>"
            "par1 = Rate  <br>"
            "par2 = Power <br>";
  }
}
