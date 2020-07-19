/* 
 * File:   xmlhandler.cpp
 * Author: robx
 * 
 * Created on September 7, 2009, 7:48 AM
 */

#include "xmlhandler.h"

#include <iostream>
using namespace std;

void getFullName(xmlNodePtr node, string &fullname);
int walk(xmlNodePtr node, map<string, Parameter> &elements);
int updateElement_(xmlDocPtr doc, const xmlChar* xpathExpr, const xmlChar* value);

XmlHandler::XmlHandler(const string filename) {
    xmlfilename = filename;
}

XmlHandler::~XmlHandler() {
    xmlFreeDoc(doc);
}

int XmlHandler::updateElement(string name, string value) {
    cout << "name:" << name.c_str() << endl;
    updateElement_(doc, BAD_CAST name.c_str(), BAD_CAST value.c_str());
    return 0;
}

int XmlHandler::test() {
    xmlDocPtr doc;

    doc = xmlParseFile("config2.xml");
    map<string, Parameter> parameters;
    walk(doc->children, parameters);
    string parametername = "/robix_configuration/serialport/linux/motor";
    parameters[parametername].stringValue = "/sd/fg";
    updateElement_(doc, BAD_CAST parametername.c_str(), BAD_CAST parameters[parametername].stringValue.c_str());
    //updateElement_(doc, BAD_CAST "/robix_configuration/serialport/linux/motor", BAD_CAST "23");
    //updateElement_(doc, BAD_CAST "/robix_configuration/hue/lower-limit", BAD_CAST "123");
    xmlSaveFormatFileEnc("config2.xml", doc, "UTF-8", 1);

    xmlFreeDoc(doc);
    return 0;
}

int XmlHandler::load(map<string, Parameter> &elements) {
    doc = xmlParseFile(xmlfilename.c_str());
    return walk(doc->children, elements);
}

int XmlHandler::save() {
    // if xmlSaveFormatFileEnc then save did not work properly for multilevel parameters
    //xmlSaveFormatFileEnc(xmlfilename.c_str(), doc, "UTF-8", 1);
    return 0;
}

/*****************************************************************************/

void getFullName(xmlNodePtr node, string &fullname) {
    assert(node != NULL);
    if (node->parent != NULL) {
        getFullName(node->parent, fullname);
    } else {
        fullname = "";
    }
    if (node->name != NULL) {
        fullname = fullname + "/" + (const char *) (node->name);
    }
}

int walk(xmlNodePtr node, map<string, Parameter> &elements) {
    xmlNodePtr node1;
    for (node1 = node;
            node1 != NULL;
            node1 = node1->next) {

        walk(node1->children, elements);
        if (node1->type == XML_ELEMENT_NODE && node1->parent != NULL && node1->parent->name != NULL &&
                (node1->children == NULL || node1->children->type == XML_TEXT_NODE)) {
            string fullname;
            getFullName(node1, fullname);
            string value = (const char *) xmlNodeGetContent(node1);
            int intValue;
            double doubleValue;
            stringstream ss(value);
            stringstream ss2(value);
            Parameter parameter;
            if ((ss >> doubleValue).fail()) {
                parameter.type = STRING;
                parameter.stringValue = value;
                //cout << fullname << " loaded with s: " << value << ":" << endl;
            } else if ((ss2 >> intValue).fail() || intValue != doubleValue ) {
                parameter.type = DOUBLE;
                parameter.doubleValue = doubleValue;
                //cout << fullname << " loaded with d: " << doubleValue << ":" << endl;
            } else {
                parameter.type = INT;
                parameter.intValue = intValue;
                //cout << fullname << " loaded with i: " << intValue << ":" << endl;
            }
            elements[fullname] = parameter;
        }
    }
    return 1;
}

/**
 * update_xpath_nodes:
 * @nodes:		the nodes set.
 * @value:		the new value for the node(s)
 *
 * Prints the @nodes content to @output.
 */
static void
update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar* value) {
    int size;
    int i;

    assert(value);
    size = (nodes) ? nodes->nodeNr : 0;

    /*
     * NOTE: the nodes are processed in reverse order, i.e. reverse document
     *       order because xmlNodeSetContent can actually free up descendant
     *       of the node and such nodes may have been selected too ! Handling
     *       in reverse order ensure that descendant are accessed first, before
     *       they get removed. Mixing XPath and modifications on a tree must be
     *       done carefully !
     */
    for (i = size - 1; i >= 0; i--) {
        assert(nodes->nodeTab[i]);

        xmlNodeSetContent(nodes->nodeTab[i], value);
        /*
         * All the elements returned by an XPath query are pointers to
         * elements from the tree *except* namespace nodes where the XPath
         * semantic is different from the implementation in libxml2 tree.
         * As a result when a returned node set is freed when
         * xmlXPathFreeObject() is called, that routine must check the
         * element type. But node from the returned set may have been removed
         * by xmlNodeSetContent() resulting in access to freed data.
         * This can be exercised by running
         *       valgrind xpath2 test3.xml '//discarded' discarded
         * There is 2 ways around it:
         *   - make a copy of the pointers to the nodes from the result set
         *     then call xmlXPathFreeObject() and then modify the nodes
         * or
         *   - remove the reference to the modified nodes from the node set
         *     as they are processed, if they are not namespace nodes.
         */
        if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            nodes->nodeTab[i] = NULL;
    }
}

int updateElement_(xmlDocPtr doc, const xmlChar* xpathExpr, const xmlChar* value) {
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        fprintf(stderr, "Error: unable to create new XPath context\n");
        xmlFreeDoc(doc);
        return (-1);
    }

    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if (xpathObj == NULL) {
        fprintf(stderr, "Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return (-1);
    }
    update_xpath_nodes(xpathObj->nodesetval, value);

    return 0;
}
