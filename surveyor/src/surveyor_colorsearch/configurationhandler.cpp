/* 
 * File:   Configuration.cpp
 * Author: robx
 * 
 * Created on September 3, 2009, 5:31 PM
 */

#include "xmlhandler.h"

#include "configurationhandler.h"

Configuration::Configuration(string configurationSource) {

    xmlHandler = new XmlHandler(configurationSource);
    if (!xmlHandler->load(parameters)) {
        cout << "Parameter loading problem!" << endl;
    }
}

Configuration::~Configuration() {
    saveConfiguration();
    delete xmlHandler;
}

int Configuration::saveConfiguration() {
    map<string, Parameter>::iterator iter;
    for (iter = parameters.begin(); iter != parameters.end(); ++iter) {
        std::stringstream out;
        if (iter->second.type == INT) {
            out << iter->second.intValue;
        } else if (iter->second.type == DOUBLE) {
            out << iter->second.doubleValue;
        } else {
            out << iter->second.stringValue;
        }
        xmlHandler->updateElement(iter->first, out.str());
    }
    return xmlHandler->save();
}

bool Configuration::hasParameterWithType(string parameterName, ParameterType type) {
    return parameters.find(CONFIGURATION_PREFIX + parameterName) != parameters.end() &&
            parameters[CONFIGURATION_PREFIX + parameterName].type == type;
}

int* Configuration::getIntParamRef(string parameterName) {
    if (!hasParameterWithType(parameterName, INT)) {
        cout << "ERROR - Unknown parameter:" << parameterName << endl;
        return NULL;
    } else {
        return &parameters[CONFIGURATION_PREFIX + parameterName].intValue;
    }
}

int Configuration::getIntParam(string parameterName) {
    int *i = getIntParamRef(parameterName);
    if (i != NULL) {
        return *i;
    }
    return 0;
}

void Configuration::setIntParam(string parameterName, int value) {
    if (hasParameterWithType(parameterName, INT)) {
        parameters[CONFIGURATION_PREFIX + parameterName].type = INT;
        parameters[CONFIGURATION_PREFIX + parameterName].intValue = value;
    } else {
        cout << "ERROR - Unknown parameter:" << parameterName << endl;
    }
}

string* Configuration::getStringParamRef(string parameterName) {
    if (!hasParameterWithType(parameterName, STRING)) {
        cout << "ERROR - Unknown parameter:" << parameterName << endl;
        return NULL;
    } else {
        return &parameters[CONFIGURATION_PREFIX + parameterName].stringValue;
    }
}

string Configuration::getStringParam(string parameterName) {
    string *s = getStringParamRef(parameterName);
    if (s != NULL) {
        return *s;
    }
    return "";
}

void Configuration::setStringParam(string parameterName, string value) {
    if (hasParameterWithType(parameterName, STRING)) {
        parameters[CONFIGURATION_PREFIX + parameterName].type = STRING;
        parameters[CONFIGURATION_PREFIX + parameterName].stringValue = value;
    } else {
        cout << "ERROR - Unknown parameter:" << parameterName << endl;
    }
}

double* Configuration::getDoubleParamRef(string parameterName) {
    // all int parameters are saved as doubles because it is not known if they are accidentally fit in integer

    if (!hasParameterWithType(parameterName, DOUBLE) && !hasParameterWithType(parameterName, INT)) {
        cout << "ERROR - Unknown parameter:" << parameterName << endl;
        return NULL;
    } else {
        if( hasParameterWithType(parameterName, INT) ) {
            // an integer parameter is referenced as double it has to be converted
            parameters[CONFIGURATION_PREFIX + parameterName].type = DOUBLE;
            parameters[CONFIGURATION_PREFIX + parameterName].doubleValue =
                    parameters[CONFIGURATION_PREFIX + parameterName].intValue;
        }
        return &parameters[CONFIGURATION_PREFIX + parameterName].doubleValue;
    }
}

double Configuration::getDoubleParam(string parameterName) {
    double *d = getDoubleParamRef(parameterName);
    if (d != NULL) {
        return *d;
    }
    return 0;
}

void Configuration::setDoubleParam(string parameterName, double value) {    
    if (hasParameterWithType(parameterName, INT) || hasParameterWithType(parameterName, DOUBLE)) {
        parameters[CONFIGURATION_PREFIX + parameterName].type = DOUBLE;
        parameters[CONFIGURATION_PREFIX + parameterName].doubleValue = value;
    } else {
        cout << "ERROR - Unknown parameter:" << parameterName << endl;
    }
}
