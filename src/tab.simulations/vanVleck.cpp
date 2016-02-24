/***************************************************************************
*  File: simvanvleck.cpp                                                   *
*  Description: performs dipolar line width calculation based on vanVleck  *
*  Author: Samir Tabbara                                                   *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: simvanvleck.cpp 2004-01-00 $                                       *
***************************************************************************/
/***************************************************************************
    This file is part of BnmrOffice.

    BnmrOffice is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BnmrOffice  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BnmrOffice .  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

/***************************************************************************
List of Changes:
----------------------------------------------------------------------------
TYPE  DATE        AUTHOR  DESCRIBE
MI
***************************************************************************/




#include "vanVleck.h"


/*** Function Compare used to compare two element ***/
// return : < 0: *a goes before *b
//        : = 0: *a == *b
//        : > 0: *a goes after *b
int compare (const void * a, const void * b)
{
  if ( ((*(vec3 *)a) * (*(vec3 *)a)) <  ((*(vec3 *)b) * (*(vec3 *)b)) )    return -1;
  if ( ((*(vec3 *)a) * (*(vec3 *)a)) ==  ((*(vec3 *)b) * (*(vec3 *)b)) )   return  0;
  if ( ((*(vec3 *)a) * (*(vec3 *)a)) >  ((*(vec3 *)b) * (*(vec3 *)b)) )    return  1;
}

/************************************************************************************/
/*** Function that finds the nearest neighbours ***/
// Passing the primitive vectors, the implanted atom
// and the radius of the sphere that includes the nearest neighbours
// return the nearest neighbour sorted from closest to farsest
void nearest_neighbours ( const vec3 a1, const vec3 a2, const vec3 a3, const vec3 implanted_atom,
                          const float boundary_length, int& counter, vec3 nearest_neighbour_sorted[1000],
                          vec3 & return_implanted_atom )
{
  int u1 = 0, u2 = 0, u3 = 0;      // arbitrary integers
  vec3 lat_pts(0,0,0);
  vec3 distance(0,0,0);
  return_implanted_atom = implanted_atom;
  for ( u3 = -10; u3 <= 10; u3++ )
  {
    for ( u2 = -10; u2 <= 10; u2++ )
    {
      for ( u1 = -10; u1 <= 10; u1++ )
      {
        // Kittel, "Introduction to Solid State" equ.1, p.4
        lat_pts  =  ( u1 * a1 ) + ( u2 * a2 ) + ( u3 * a3 );
        distance = lat_pts - implanted_atom;
        if  (( distance.length() <= boundary_length ) && ( lat_pts != implanted_atom ))
      {
            nearest_neighbour_sorted[counter] = distance;
            counter++;
      }
      }
    }
  }
  // Sorting the nearest neighbours
  // Using the distance between implanted atom and nearest neighbour for sorting
  qsort (nearest_neighbour_sorted, counter, sizeof(vec3), compare);
  for ( int i = 0; i < counter; i++ )// Reverting to lattice pts
    nearest_neighbour_sorted[i] = nearest_neighbour_sorted[i] + implanted_atom;
}


/**********************************************************************************/
/*** Function that counts the nearest neighbour for each level ***/
// return number of nearest neighbour levels
int counting_nearest_neighbour_per_level (const vec3 nearest_neighbour_sorted[1000],
                      int nearest_neighbour_level_counting[100],
                      const int counter,
                      const vec3 return_implanted_atom)
{
  int i = 0;
  for (int j = 0; j < counter; j++)
    {
      (nearest_neighbour_level_counting[i])++ ;
      if  ( ( (nearest_neighbour_sorted[j+1] - return_implanted_atom).length() )
        !=  ( (nearest_neighbour_sorted[j] - return_implanted_atom).length() ) )
    i++;
    }
  return i;
}
/*****************************************************************/
/*** Function that calculates the Second Moment of Unlike spins ***/
// Check p.123 "Principle of Nuclear Magnetism,ABRAGAM"
double second_moment_unlike_spin(const double  gamma_i, const double gamma_l, const float s,
                 const int     nearest_neighbour_level_counting[100],
                 const vec3    nearest_neighbour_sorted[1000],
                 const vec3    return_implanted_atom, double theta,double phi,double powder,
                 const double lattice_ct, const int level_rank)
{
  double  second_moment = 0.0;
  vec3    r_unlike_spin_vector(0,0,0);
  double  cos_alpha = 0.0; // -cosine of the angle between magnetic field and the vector relating
                           //  the spin of implanted atom and that of nearest neighbour
  int remember_j_counter = 0;
  vec3 field(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta) );  // Magnetic filed H
  vec3 h_0 = field; // Tesla *
  int i = 0;
  int j = 0;
  int k = 0;
  // Loop for iterating accross levels of nearest neighbours
  while (i <= level_rank)
    {
      k = nearest_neighbour_level_counting[i] + j;
      //cout << "remember_j_counter: " << remember_j_counter << endl;
      //cout << "nearest_neighbour_level_counting: " << nearest_neighbour_level_counting[i]<< endl;
      // Loop for iterating accross same level nearest neighbours
      while (j < k)
    {
      r_unlike_spin_vector = lattice_ct * (nearest_neighbour_sorted[j] -  return_implanted_atom);
      cos_alpha = (1.0-powder)*( h_0 * r_unlike_spin_vector ) / ( h_0.length() * r_unlike_spin_vector.length() );
      second_moment = second_moment
        + (1.0/3.0) * pow( gamma_i * gamma_l, 2. ) * ( s * (s + 1.0) ) * pow( h_bar, 2. )
        * pow(1 - (3 * pow(cos_alpha, 2.)), 2. )
        * pow( r_unlike_spin_vector.length(), -6. )
        * pow( mu_0, 2. );
      //cout  << "nearest_neighbour_sorted[j] :"<<  nearest_neighbour_sorted[j]<< endl;
      j++;
      //remember_j_counter = j;
    }
      i++;
    }
  return second_moment;
}
//Vleck_sim(implanted,target,type_lattice,X,Y,Z);
vector< vector<double> > Vleck_sim(int implanted, int target, int type_of_lattice,
                                   double x,double y, double z)
{
    int    lattice_str = 0; double gamma_l = 0.0, lattice_ct = 0.0, s = 0.0;
    
    //Definitions:
    //gamma_l=Sum_i[f_i gn*μN/hbar], where f_i is the abundance 
    //gn is the nuclear g factor (given is //http://www.easyspin.org/documentation/isotopetable.html)
    //μN= 5.05078353×10−27 J/T  is the nulcear magneton
    //hbar=1.054571800*10^-34 JS is the planck cst.
    //μN/hbar=47.893882956 MHzT
    //The values below are given in s-1 T
    //for details see the python script vanvleck.py

    switch( target )
    {
    case 0:  lattice_str = 1; gamma_l = 72554195.19;  lattice_ct =3.6149; s=1.5;break;//<Copper>
    case 1:  lattice_str = 2; gamma_l = 65672092.30;  lattice_ct =3.3004; s=4.5;break;//<Niobium>
    case 2:  lattice_str = 1; gamma_l = 69762364.01;  lattice_ct =4.0495; s=2.5;break;//<Aluminum>
    case 3:  lattice_str = 1; gamma_l =-11662405.13;  lattice_ct =4.0853; s=0.5;break;//<Silver>
    case 4:  lattice_str = 1; gamma_l = 4653561.24;   lattice_ct =4.0782; s=1.5;break;//<Gold>
    case 5:  lattice_str = 1; gamma_l = 58382643.32;  lattice_ct =3.9242; s=0.5;break;//Pt!!
    case 6:  lattice_str = 1; gamma_l =-12308727.91;  lattice_ct =3.890;  s=2.5;break;//Pd!!
    case 7:  lattice_str = 1; gamma_l = 56759998.56;  lattice_ct =4.9508; s=0.5;break;//Lead!!
    }
    lattice_ct = lattice_ct*AN;
    double x1 = 0,y1 = 0,z1 = 0,x2 = 0,y2 = 0,z2 = 0,x3 = 0,y3 = 0,z3 = 0; // Primitive vectors
    switch( lattice_str )
    {
    case 0:x1=1.0; y1= 0.0; z1= 0.0; x2= 0.0;y2= 1.0; z2= 0.0; x3= 0.0;y3 =  0.0;z3= 1.0;break;/*** sc ***/
    case 1:x1=0.5; y1= 0.5; z1= 0.0; x2= 0.5;y2= 0.0; z2= 0.5; x3= 0.0;y3 =  0.5;z3= 0.5;break;/*** fcc ***/
    case 2:x1=0.5; y1= 0.5; z1=-0.5; x2=-0.5;y2= 0.5; z2= 0.5; x3= 0.5;y3 = -0.5;z3= 0.5;break;/*** bcc ***/
    case 3:x1=0.5; y1= 0.5; z1=-0.5; x2=-0.5;y2= 0.5; z2= 0.5; x3= 0.5;y3 = -0.5;z3= 0.5;break;/**Diamond!!*/
    case 4:x1=0.5; y1= 0.5; z1=-0.5; x2=-0.5;y2= 0.5; z2= 0.5; x3= 0.5;y3 = -0.5;z3= 0.5;break;/*SaltRock!!*/
    }
    /*** Implanted atom ***/

    double gamma_i = 0.0;
    switch( implanted )
    {
    //http://bnmr.triumf.ca/?file=Introduction
    //
    case 0: gamma_i      = 6301500.; break; //Li-8
    case 1: gamma_i      = 135534200;break; //muon
    case 2: gamma_i      = 22000000; break; //Be-11
    case 3: gamma_i      = 267520607.23; break; //protons
    case 4: gamma_i      = 74483671.04; break; //Boron
    case 5: gamma_i      = 72554195.19; break; //Cu
    case 6: gamma_i      = 65672092.30; break; //Niobium
    case 7: gamma_i      = 69762364.01; break; //Aluminium
    case 8: gamma_i      =-11662405.13; break; //Silver
    case 9: gamma_i      = 4653561.24;  break; //Gold
    }
    /*** Read Input Sites for implanting***/
    float X_IMPL = x, Y_IMPL= y, Z_IMPL= z;

    // create graph and assign data to it:

    vec3 pt(X_IMPL,Y_IMPL,Z_IMPL);
    int counter = 0;                                // modified by nearest_neighbours() ft.
    vec3 zero_vector(0,0,0);
    vec3 nearest_neighbour_sorted[1000];            // modified by nearest_neighbours() ft.
    nearest_neighbour_sorted[0]   = zero_vector;
    vec3 return_implanted_atom    = zero_vector;   // modified by nearest_neighbours() ft.
    int nearest_neighbour_level_counting[100]={0};     // modified counting_nearest_neighbour_per_level () ft.

    //nearest_neighbour_level_counting[i] = 0;
    vec3 a1(x1,y1,z1);
    vec3 a2(x2,y2,z2);
    vec3 a3(x3,y3,z3);

    /*** Seeking for nearest neighbours ***/
    nearest_neighbours  ( a1, a2, a3, pt, 3.5,
              counter, nearest_neighbour_sorted,
              return_implanted_atom);

    /*** Counting number of nearest neighbour per level ***/
    int max_level = counting_nearest_neighbour_per_level  ( nearest_neighbour_sorted,
                                nearest_neighbour_level_counting,
                                counter,
                                return_implanted_atom );


    //storing vector
    vector< vector<double> > simulatedData;
    simulatedData.push_back( vector<double> () );
    simulatedData.push_back( vector<double> () );

    /*** Calculating Dipolar Line Width ***/
    double  phi     = PI/4.0;  // '' '' and [(1,0,0) direction] in [(001) plane] 0 < phi   < pi/4 (due to symmetry)
    int     level_rank = 10;
    double  second_moment = 0.0;

    if (type_of_lattice == 0)//for crystal lattice
    {
        // -angle between magnetic field and [(0,0,1) direction] in[(110)plane],0<theta<pi/2
        for (double theta = 0; theta <= 90; theta += 1.0)
        {
            second_moment = second_moment_unlike_spin(gamma_i, gamma_l, s,
                                                      nearest_neighbour_level_counting,nearest_neighbour_sorted,
                                                      return_implanted_atom,theta * (PI/180.0),phi ,0.0,
                                                      lattice_ct, level_rank);
            simulatedData[0].push_back(theta);
            simulatedData[1].push_back(sqrt(second_moment)/UNIT);
        }
    }
    if (type_of_lattice == 1)//for powder
    {
        for (double theta = 0; theta <= 90; theta += 1.0)
        {
            second_moment = second_moment_unlike_spin(gamma_i, gamma_l, s,
                                                      nearest_neighbour_level_counting,nearest_neighbour_sorted,
                                                      return_implanted_atom,(90.) * (PI/180.0),phi,1.0,
                                                      lattice_ct, level_rank);
            simulatedData[0].push_back(theta);
            simulatedData[1].push_back(sqrt(second_moment*4.0/5.0)/UNIT);
        }
    }
    return simulatedData;
}


