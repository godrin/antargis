/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_xml.h
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#ifndef __MY_SIMPLE_XML
#define __MY_SIMPLE_XML

#include <rk_tools.h>
#include <rk_utf8.h>
#include "rk_debug.h"

#include <vector>
#include <map>
#include <ag_stringstream.h>
#include <sstream>


struct XMLParseError
  {
    XMLParseError(const std::string &p):problem(p){}
    std::string problem;
  };

class AGEXPORT Node
  {
  public:
    typedef std::map<AGString,AGString> Attributes;

    typedef std::vector<Node*> NodeVector;

    typedef NodeVector::iterator iterator;
    typedef NodeVector::const_iterator const_iterator;

    Node();
    Node(const AGString &name);
    Node(const Node &n);
    Node(const Node *n);
    ~Node();

    void setName(const AGString &pName);

    NodeVector getChildren() const;

#ifndef SWIG
    const Node &(operator*)() const;
    const Node *(operator->)() const;
#endif
    NodeVector getChildren(const AGString &pName) const;

    const AGString &getName() const;

    void setAttributes(const Attributes &pAttrs);
    Attributes getAttributes() const;

    Node &addChild(const AGString &pName);

    void removeChild(Node &n);

    iterator begin();
    iterator end();
#ifndef SWIG
    const_iterator begin() const;
    const_iterator end() const;
#endif

    void setContent(const AGString &s);
    AGString getContent() const;

    void set(const AGString &pName,const AGString &pValue);
    AGString get(const AGString &pName) const;
    void clear();

    static AGString escape(const AGString &s);
    static AGString unescape(const AGString &s);

    // dumping functions
    void getStart(AGStringStream &s,bool complete=false) const;
    void getEnd(AGStringStream &s) const;

    void indent(AGStringStream &s,int depth) const;
    void getContent(AGStringStream &s,int depth) const;
    AGString toString(bool indent=true) const;

    bool isTextNode() const;
    AGString getText() const;

    /** me or direct children is text node */
    bool hasTextNode() const; 

    size_t size() const;

  private:
    AGString mName;
    std::vector<Node*> mNodes;
    Attributes mAttrs;
    AGString mContent;
  };

class AGEXPORT Document
  {
    Node *mRoot;
  public:
    Document();
    Document(const AGString &pFilename);
    ~Document();

    bool parseFile(const AGString &pFilename);

    Node &root();
    const Node &root() const;

    AGString toString(bool forceIndent=false) const;

    void parseMemory(const AGString &s);
  };

class AGEXPORT Parser
{
  struct AGEXPORT Data
  {
    std::list<size_t> stack;
    std::list<size_t> linestack;
    size_t pos;
    AGString s;
    size_t line;

    Data();

    bool first(const AGString &p) const;
    AGString getFirst(size_t i) const;
    void eat(size_t i) throw (XMLParseError);
    void push();
    void pop();
    void discard();
    AGString getTil(const AGString &p) const;
    void eatBlanks() throw (XMLParseError);
  };

  struct NodeStartInfo
  {
    bool startTag;
    bool simple; // == <.../>
    AGString name;
  };

  Data data;

  NodeStartInfo parseNodeStart();
  bool parseNodeEnd();
  void parseNodeContent();
  bool parseHeader();
  AGString parseString();
  Node::Attributes parseAttributes();
  bool parseComment();
  bool parseText();
  void parseNode();
  AGString parseName();

  bool eod();

 public:
  virtual ~Parser();

  void parse(const AGString &pData);
  size_t getLine() const;

  virtual void simpleTag(const AGString &pName,const Node::Attributes &pAttributes);
  virtual void startTag(const AGString &pName,const Node::Attributes &pAttributes);
  virtual void endTag(const AGString &pName);
  virtual void text(const AGString &pText);
  virtual void comment(const AGString &pText);
  virtual void header(const AGString &pText);
};

class AGEXPORT DomParser:public Parser
{
  Document *doc;
  std::list<Node*> nodes;
 public:
  virtual void simpleTag(const AGString &pName,const Node::Attributes &pAttributes);
  virtual void startTag(const AGString &pName,const Node::Attributes &pAttributes);
  virtual void endTag(const AGString &pName);
  virtual void text(const AGString &pText);
  virtual void comment(const AGString &pText);
  virtual void header(const AGString &pText);

  Document *parse(const AGString &pData);

  Document *parse(const AGString &pData,Document *d);
};


#endif
