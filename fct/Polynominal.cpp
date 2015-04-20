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
    defaults.addParameter( "A0" , 0.0, 0.01, 0.0, 1.0  );//par[0]
    defaults.addParameter( "A1" , 0.1, 0.01, 0.0, 1.0  );//par[1]
    defaults.addParameter( "A2" , 0.2, 0.01, 0.0, 1.0  );//par[2]
    defaults.addParameter( "A3" , 0.3, 0.01, 0.0, 1.0  );//par[3]
    defaults.addParameter( "A4" , 0.4, 0.01, 0.0, 1.0  );//par[4]
    defaults.addParameter( "A5" , 0.5, 0.01, 0.0, 1.0  );//par[5]

  }

  //This must the same number of parameters as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
    return par[0]*pow(x, 0.0) + par[1]*pow(x, 1.0)+ par[2]*pow(x, 2.0)
          + par[3]*pow(x, 3.0)+ par[4]*pow(x, 4.0)+ par[5]*pow(x, 5.0);
  }

  //Write the description here. Not critical.
  void description(string &text)
  {
    text = "Name of Function: streched exponential.<br><br>"
            "f(x) = par[0]*pow(x, 0.0) + par[1]*pow(x, 1.0)+ par[2]*pow(x, 2.0)<br>"
          "+ par[3]*pow(x, 3.0)+ par[4]*pow(x, 4.0)+ par[5]*pow(x, 5.0)<br><br>"
            "where:<br>"
            "par0 = A0<br>"
            "par1 = A1<br>"
            "par2 = A2<br>"
            "par3 = A3<br>"
            "par4 = A4<br>"
            "par5 = A5<br>";
  }
}
