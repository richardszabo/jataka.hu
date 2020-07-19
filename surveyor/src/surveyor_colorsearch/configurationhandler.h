/* 
 * File:   configurationhandler.h
 * Author: robx
 *
 * Created on September 3, 2009, 5:31 PM
 */

#ifndef _CONFIGURATIONHANDLER_H
#define	_CONFIGURATIONHANDLER_H

//#include <libxml/xmlreader.h>

#include <map>
#include <string>
#include <iostream>

using namespace std;

#include "xmlhandler.h"

#define CONFIGURATION_PREFIX "/surveyor_configuration/"

class Configuration {
public:
    Configuration(string configurationSource);
    virtual ~Configuration();
    //void loadConfiguration();
    int saveConfiguration();
    //string *getParameter(string parameterName);
    //void setParameter(string parameterName, string value);


    // returns NULL if parameterName is not found
    int* getIntParamRef(string parameterName);
    int getIntParam(string parameterName);
    void setIntParam(string parameterName, int value);
    string* getStringParamRef(string parameterName);
    string getStringParam(string parameterName);
    void setStringParam(string parameterName, string value);
    double* getDoubleParamRef(string parameterName);
    double getDoubleParam(string parameterName);
    void setDoubleParam(string parameterName,double value);

    /*
     * // returns NULL if parameterName is not found
    double *getDoubleParam(string parameterName);
    void setDoubleParam(string parameterName, double value);
     */

private:
    string configurationSource;
    map<string, Parameter> parameters;
    XmlHandler *xmlHandler;
    //map<string,int> intParameters;
    //map<string,double> doubleParameters;

    bool hasParameterWithType(string parameterName,ParameterType type);

};

#endif	/* _CONFIGURATIONHANDLER_H */

