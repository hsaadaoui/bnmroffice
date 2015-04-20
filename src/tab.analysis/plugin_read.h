#ifndef READ_FUNCTION_H
#define READ_FUNCTION_H

using namespace std;
#include <QLibrary>
#include <QStringList>
#include <QDir>

#include "parameters.h"
class function
{

public:
    function(QString);

    ~function();

    double operator()(double x,const vector<double> &) const;

    QString getLibraryDescription(void);

    bool    getLibraryState(QString);

    QStringList getLibrariesInDir(void);

    void getParameters(Parameters&);

private:
    bool loadState=false;

    QString libName;

    QString directory;

    QLibrary *fctLibrary;
};


#endif
