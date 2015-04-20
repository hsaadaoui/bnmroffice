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
    defaults.addParameter( "Amplitude1" , 0.2, 0.01, 0.0, 0.33  );//par[0]
    defaults.addParameter( "Rate1"      , 0.1, 0.01, 0.0, .2 );//par[1]
    defaults.addParameter( "Power1"     , 0.8, 0.01, 0.5, 1.5  );//par[2]
    defaults.addParameter( "Amplitude2" , 0.2, 0.01, 0.0, 0.33  );//par[3]
    defaults.addParameter( "Rate2"      , 0.1, 0.01, 0.0, .2 );//par[4]
    defaults.addParameter( "Power2"     , 0.8, 0.01, 0.5, 1.5  );//par[5]
  }

  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
    return par[0]*exp( -pow( par[1]*x, par[2] ) )
            +par[3]*exp( -pow( par[4]*x, par[5] ) );
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: two streched exponentials.<br><br>"
            "f(x) = par[0]*exp( -pow( par[1]*x, par[2] ))<br><br>"
             "+par[3]*exp( -pow( par[4]*x, par[5] ) )<br><br>"
            "where:<br>"
            "par0 = Amplitude1<br>"
            "par1 = Rate1  <br>"
            "par2 = Power1 <br>"
           "par3 = Amplitude2<br>"
           "par4 = Rate2  <br>"
           "par5 = Power2 <br>";
  }
}
