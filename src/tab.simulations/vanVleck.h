#ifndef VANVLECK_H
#define VANVLECK_H

#include "algebra3_m.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>                               //Header file for math operations
#include <cassert>
#include <stddef.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

const   double    PI     =   3.1415926535897932384626433832795;
const   double    h_bar  =   (6.62606876  * pow(10,-34.)) /(2.0* PI);
const   double    mu_0   =   4.0 * PI     * pow(10,-7.);
const   double    AN     =   0.0000000001;
//const   double    UNIT    =  1000.00/(4 * PI);//  1000 for khz, and 4Pi for volume
#define   UNIT    1000.0/(4.0*PI)

vector< vector<double> > Vleck_sim(int implanted, int target, int type_of_lattice,
                                 double x,double y, double z);

#endif

