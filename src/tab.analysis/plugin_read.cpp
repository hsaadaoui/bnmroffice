#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <QDebug>
#include "plugin_read.h"
#include "parameters.h"
using namespace std;

#include <QLibrary>
#include <QStringList>
#include <QDir>

//function constructor with just the name
function::function(QString Directory)
{
    directory  = Directory;
}
function::~function()
{
    //delete fctLibrary;
}

//get all libraries in the provided directory
QStringList function::getLibrariesInDir(void)
{
    //check if dir has a / at end, otherwise append it
    if( directory[directory.size()-1] != '/' )
      directory += '/';
    //get the directory listing of all lib*.so files
    QDir filterDir(directory, "lib*.so" );
    //return all libraries
    return filterDir.entryList();
}

//loadState: it is true if it has found the library.
bool function::getLibraryState(QString name)
{
    libName    = directory+name;
    fctLibrary = new QLibrary(libName);
    loadState  = fctLibrary->load();
    //generates the library name, no need to append .so at end, qlibary knows!
    if(  loadState == false)
    {
        qDebug()<< "Cannot load/find the library: "<< libName;
        qDebug()<< "Load error: "<< fctLibrary->errorString();
        delete fctLibrary;
    }
    return loadState;
}

//find the function f(x,pars)
double function::operator()(double x,const vector<double> &pars) const
{
    if( loadState==false )
    {
        qDebug()<< "Load error: "<< fctLibrary->errorString();
        delete fctLibrary;
    }
    else
    {
        typedef double (*Fct)(double, vector<double>);
        Fct function = (Fct) fctLibrary->resolve("function");
        if (function)
        {
            return function(x,pars);
        }
        else
        {
            qDebug()<< "This function makes 0..nada..walo!";
            return 0.0;
        }
    }
}

//get the default parameters
void function::getParameters(Parameters& defaults)
{
    if( loadState==false )
    {
        qDebug()<< "Load error: "<< fctLibrary->errorString();
        delete fctLibrary;
    }
    else
    {
        typedef void (*parDfts)(Parameters&);
        parDfts pardft = (parDfts) fctLibrary->resolve("defaultParameters");
        if (pardft)
        {
            pardft(defaults);
        }
        else
        {
            qDebug()<< "Something wrong with parameters section of the library";
        }
    }

}
//find the description
QString function::getLibraryDescription(void)
{
    if( loadState==false )
    {
        qDebug()<< "Load error: "<< fctLibrary->errorString();
        delete fctLibrary;
    }
    else
    {
        string desc;
        typedef void (*pD)(string&);
        pD plugDescribe = (pD) fctLibrary->resolve("description");
        if( plugDescribe )
        {
            plugDescribe(desc);
            return QString::fromStdString(desc);
        }
        else
        {
            return "This fct library has no description! Write it.";
        }
    }
}
