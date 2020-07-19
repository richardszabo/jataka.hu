/* 
 * File:   xmlhandler.h
 * Author: robx
 *
 * Created on September 7, 2009, 7:48 AM
 */

#ifndef _XMLHANDLER_H
#define	_XMLHANDLER_H

#include <string>
#include <map>
#include <sstream>

using namespace std;

#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
//#include <string.h>
#include <assert.h>

typedef enum  {
    INT, STRING, DOUBLE
} ParameterType;

class Parameter {
public:
    ParameterType type;
    int intValue;
    string stringValue;
    double doubleValue;
};

class XmlHandler {
public:
    XmlHandler(const string filename);
    int load(map<string,Parameter> &elements);
    int updateElement(string name,string value);
    int save();
    virtual ~XmlHandler();
    int test();
private:
    string xmlfilename;
    xmlDocPtr doc;
};

#endif	/* _XMLHANDLER_H */

