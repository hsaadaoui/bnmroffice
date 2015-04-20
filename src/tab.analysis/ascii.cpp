/***************************************************************************
*  File: ascii.cpp                                                         *
*  Description: handling ascii ascii files.                                *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: ascii.h 2014-12-20 $                                               *
*  Copyright (C) 2011 by Hassan Saadaoui                                   *
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
***************************************************************************/

//C++ and C headers
#include<iostream>
#include<fstream>
#include<math.h>
#include<cassert>
#include<string>
#include<sstream>
#include<time.h>
#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<vector>
#include<unistd.h>
#include<stdarg.h>
#include<iomanip>
#include<iterator>
#include<ctype.h>
#include<stdlib.h>
#include<strings.h>
#include<errno.h>
using namespace std;
#include "ascii.h"

//read .inf files
QStringList ASCII::read_infofiles(QStringList &headers,
                                 QVector< QVector <QString> > &headerValue)
{
    QStringList files;
    QFile inFile(Input);
    inFile.open(QFile::ReadWrite);
    if(inFile.exists())
    {
        QTextStream inData(&inFile);
        QStringList result= inData.readLine().split(QRegExp("\\s"));
        foreach (const QString &str, result)
        {
            if (!str.isEmpty())
                headers<< str;
        }
        int i  = 0;
        while (true )
        {
            QString lines = inData.readLine();
            //read lines but discard the ones starting with  #, ! or //
            if (lines[0] != '#' &&  lines[0] != '!' &&  lines[0] != '/' )
            {
                headerValue.push_back(QVector<QString>());
                QStringList results= lines.split(QRegExp("\\s"));
                foreach (const QString &str, results)
                {
                    if (!str.isEmpty())
                        headerValue[i].push_back(str);
                }
                files << headerValue[i][0];
                i++;
            }
            if( inData.atEnd() ) break;
        }
    }
    else
    {
        cout <<"\n\nCannot find/open "<<Input.toStdString()<<" file\n\n";
    }
    inFile.close();
    return files;
}

//convert the input into a run numbers list
QStringList  ASCII::read_listfiles(QStringList &headers,
                                    QVector< QVector <QString> > &headerValue)
{
    QStringList files;
    //Create a temporary file which would contains the files to fit with headers
    QFile inFile(Input);
    inFile.open(QFile::ReadOnly);
    if(inFile.exists())
    {
        QTextStream inData(&inFile);
        QString input = inData.readLine();
        headers   <<  "FileName";
        files = input.split(QRegExp(","));
        headerValue.resize(files.size());
        for(short i = 0; i < files.size(); i++)
            headerValue[i].push_back( files.at(i) );
    }
    else
    {
        qDebug() <<"\n\nCannot find/open "<<Input<<" file\n\n";
    }
    inFile.close();
    return files;
}

//contruct the files list
QStringList  ASCII::files(QStringList &headers,
                          QVector< QVector <QString> > &headerValue)
{
    QStringList files;
    headers.clear();
    headerValue.clear();
    if (QFileInfo(Input).suffix()=="inf")
    {
        files = read_infofiles(headers,headerValue);
    }
    else if (QFileInfo(Input).suffix()=="list")
    {
        files = read_listfiles(headers,headerValue);
    }
    else
    {
        headers   <<  "FileName";
        files = Input.split(QRegExp(","));
        headerValue.resize(files.size());
        for(short i = 0; i < files.size(); i++)
            headerValue[i].push_back( files.at(i) );
    }
    //qDebug()<<files<<headers<<headerValue;
    return files;
}
