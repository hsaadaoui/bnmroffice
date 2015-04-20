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


/*Instructions==================================================================

-Make your changes to this file.

-Rename the file (eg: newfct.cpp) and save it in the folder fct/.

-cd to the directory fct, and run the script ./compile as root
  $ sudo ./compile name   (eg: sudo ./make newfct.cpp)

-It compiles the library and puts a copy in /opt/bnmroffice/lib/

==============================================================================*/

//Wrap in "C" for the compiler.
extern "C" 
{ 

  //The defalt initial parameters loaded to the table.
  void defaultParameters(Parameters &defaults)
  {
    defaults.addParameter( "Amplitude" , 0.2, 0.001, 0.15, 1.0 );//par[0]
    defaults.addParameter( "Rate"      , 0.1, 0.001, 0.0,  1.0 );//par[1]
  }

  //This must have the same number of parameters;
  //as described in the default parameters above.
  double function(double x, const std::vector<double> par)
  {
  	return par[0]*exp( - par[1]*x);//par[0] is amplitude and par[1] is rate.
  }

  //Write the description here. 
  void description(string &text)
  {
    text = "Name of Function: exponential.<br><br>"
            "f(x) = par[0]*exp( -par[1]*x )<br><br>"
            "where:<br>"
            "par0 = Amplitude<br>"
            "par1 = Rate  <br>";
  }
}
