#ifndef PARAMETERS_H
#define PARAMETERS_H

class Parameters
{
public:
    Parameters(vector<string> &names, const vector<double> &values,
               const vector<double> &errors, const vector<double> &mins, const vector<double> &maxs):
        Names(names),Values(values),Errors(errors),Mins(mins),Maxs(maxs) {};
    Parameters(){};
    ~Parameters(){};

    void addParameter(string name, double value, double error, double min, double max)
    {
        Names.push_back(name);
        Values.push_back(value);
        Errors.push_back(error);
        Mins.push_back(min);
        Maxs.push_back(max);
    }
    vector<string> parNames()
    {
        return Names;
    }
    vector<double> parValues()
    {
        return Values;
    }
    vector<double> parErrors()
    {
        return Errors;
    }
    vector<double> parMins()
    {
        return Mins;
    }
    vector<double> parMaxs()
    {
        return Maxs;
    }

private:
    string FctName;
    vector<string> Names;
    vector<double> Values;
    vector<double> Errors;
    vector<double> Mins;
    vector<double> Maxs;
};
#endif // PARAMETERS_H

