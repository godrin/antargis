/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_xml.cc
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

#include "ag_xml.h"
#include "ag_fs.h"

#include "rk_debug.h"
#include "rk_rubyobj.h"

#include "ag_profiler.h"

/**
   Node is used for xml-tags, but not for attributes (as in dom)
 */
Node::Node()
  {
  }

/**
 *
 */
Node::Node(const AGString &name):mName(name)
{
}

Node::Node(const Node &n):mName(n.mName),mAttrs(n.mAttrs),mContent(n.mContent)
{
  // copy nodes
  NodeVector::const_iterator i=n.mNodes.begin();
  for(;i!=n.mNodes.end();i++)
    mNodes.push_back(new Node(**i));
}


/// somehow needed for swig-wrapping
Node::Node(const Node *n):mName(n->mName),mAttrs(n->mAttrs),mContent(n->mContent)
{
  // copy nodes
  NodeVector::const_iterator i=n->mNodes.begin();
  for(;i!=n->mNodes.end();i++)
    mNodes.push_back(new Node(**i));
}

Node::~Node()
  {
    NodeVector::iterator i=mNodes.begin();
    for(;i!=mNodes.end();i++)
      checkedDelete(*i);
    mNodes.clear();
  }

void Node::setAttributes(const Attributes &pAttrs)
  {
    mAttrs=pAttrs;
  }

Node::Attributes Node::getAttributes() const
{
  return mAttrs;
}



void Node::setName(const AGString &pName)
  {
    mName=pName;
  }

Node::NodeVector Node::getChildren() const
{
  return mNodes;
}

const Node &Node::operator*() const
{
  return *this;
}
const Node *Node::operator->() const
{
  return this;
}

Node::NodeVector Node::getChildren(const AGString &pName) const
{
  NodeVector l;
  NodeVector::const_iterator i=mNodes.begin();
  for(;i!=mNodes.end();i++)
    {
      if((*i)->getName()==pName)
        l.push_back(*i);
    }
  return l;
}

const AGString &Node::getName() const
{
  return mName;
}

Node &Node::addChild(const AGString &pName)
  {
    Node *node=new Node(pName);
    mNodes.push_back(node);
    return *mNodes.back();
  }

void Node::removeChild(Node &)
  {
    std::cerr<<"Warning: everything is deleted - maybe only one should be! (in xml.cpp Node::remove_child(Node &n)"<<std::endl;
    clear();
  }

Node::iterator Node::begin()
  {
    return mNodes.begin();
  }
Node::const_iterator Node::begin() const
{
  return mNodes.begin();
}
Node::iterator Node::end()
  {
    return mNodes.end();
  }
Node::const_iterator Node::end() const
{
  return mNodes.end();
}

void Node::setContent(const AGString &s)
  {
    mContent=s;
  }

void Node::set(const AGString &pName,const AGString &pValue)
  {
    mAttrs[pName]=pValue;
  }
AGString Node::get(const AGString &pName) const
{
  std::string v;
  std::map<AGString,AGString>::const_iterator i=mAttrs.find(pName);
  if(i!=mAttrs.end())
    return i->second;
  return "";
}

void Node::clear()
  {

    mNodes.clear();
    mAttrs.clear();
    mName="";
  }

AGString Node::escape(const AGString &s)
  {
    return s.replace("\\","\\\\").replace("\"","\\\"");
  }

AGString Node::unescape(const AGString &s)
  {
    AGString n;
    size_t i;
    bool escape=false;
    for(i=0;i<s.length();i++)
      {
        if(s[i]=='\\')
          {
            if(escape)
              {
                escape=false;
                n+="\\";
              }
            else
              escape=true;
          }
        else if(s[i]=='\"')
          {
            if(escape)
              {
                n+="\"";
                escape=false;
              }
            else
              throw std::string("Error in unescaping");
          }
        else
          {
            escape=false;
            n+=s[i];
          }
      }
    return n;
  }

void Node::getStart(AGStringStream &s,bool complete) const
{
  s<<"<"<<mName;
  if(mAttrs.size()>0)
    {
      std::map<AGString,AGString>::const_iterator i=mAttrs.begin();
      for(;i!=mAttrs.end();i++)
        s<<" "<<i->first<<"=\""<<escape(i->second)<<"\"";
    }
  if(complete)
    s<<"/";
  s<<">";
}
void Node::getEnd(AGStringStream &s) const
{
  s<<"</"<<mName<<">";
}

size_t Node::size() const
{
  return mNodes.size();
}

AGString Node::getContent() const
{
  if(mContent.length()==0)
    {
      if(size()==1)
        if((*mNodes.begin())->mContent.length()>0)
          return (*mNodes.begin())->mContent;
    }
  return mContent;
}

void Node::indent(AGStringStream &s,int depth) const
{
  for(;depth>0;depth--)
    s<<" ";
}

bool Node::isTextNode() const
{
  return mName.length()==0 && mAttrs.size()==0;
}
AGString Node::getText() const
{
  return mContent;
}

bool Node::hasTextNode() const
{
  if(isTextNode())
    return true;

  NodeVector::const_iterator i=mNodes.begin();
  for(;i!=mNodes.end();i++)
    {
      Node *n=*i;
      if(n->hasTextNode())
        return true;
    }
  return false;
}



void Node::getContent(AGStringStream &s,int depth) const
{
  NodeVector::const_iterator i=mNodes.begin();
  for(;i!=mNodes.end();i++)
    {
      Node *n=*i;
      if(n->isTextNode())
        s<<n->getText();
      else
        {
          if(depth>0)
            {
              // width indenting
              indent(s,depth);
              if(n->mNodes.size()==0 && n->mContent.length()==0)
                n->getStart(s,true);
              else
                {
                  n->getStart(s);
                  s<<"\n";
                  n->getContent(s,depth+2);
                  indent(s,depth);
                  n->getEnd(s);
                }
              s<<"\n";

            }
          else
            {
              // without indenting
              n->getStart(s);
              n->getContent(s,0);
              n->getEnd(s);
            }
        }
    }
  s<<mContent;
}

AGString Node::toString(bool indent) const
{
  AGStringStream os;
  //    cdebug(mName.length());
  if(mName.length()==0)
    return mContent;

  if(indent)
    {
      getStart(os);
      os<<"\n";
      getContent(os,2);
      getEnd(os);
      os<<"\n";
    }
  else
    {
      getStart(os);
      getContent(os,0);
      getEnd(os);
    }
  return os.str();
}

////////////////////////////////////////////////////////////////////////
// Document
////////////////////////////////////////////////////////////////////////


Document::Document()
  {
    mRoot=new Node;
  }
Document::Document(const AGString &pFilename)
  {
    mRoot=new Node;
    mRoot->clear();
    parseFile(pFilename);
  }

Document::~Document()
  {
    checkedDelete(mRoot);
  }

bool Document::parseFile(const AGString &filename)
  {
    AGString s;
    s=loadFile(filename);

    parseMemory(s);
    return true;
  }

Node &Document::root()
  {
    return *mRoot;
  }

const Node &Document::root() const
{
  return *mRoot;
}


/*
Node *Document::getRootNode()
{
  return mRoot;
  }*/

AGString Document::toString(bool forceIndent) const
{
  if(mRoot->hasTextNode() && forceIndent==false)
    return mRoot->toString(false);
  else
    return mRoot->toString();
}
/*
void Document::parseMemory(const AGString &s)
{
  parseMemory(s);
}
 */

void Document::parseMemory(const AGString &s)
  {
    DomParser p;
    p.parse(s,this);
  }

/*Document *Document::get_document()
{
  return this;
  }*/




/******************************************************************
 * Parser
 ******************************************************************/

Parser::Data::Data()
  {
    pos=0;
    line=1;
  }


bool Parser::Data::first(const AGString &p) const
{
  if(s.length()>=p.length()+pos)
    {
      return (s.substr(pos,p.length())==p);
    }
  return false;
}

AGString Parser::Data::getFirst(size_t i) const
{
  STACKTRACE;
  if(pos>=s.length())
    return "";
  else
    return s.substr(pos,std::min(s.length()-pos,i));

  AGStringStream os;
  for(size_t k=pos;k<i+pos;k++)
    {
      if(k>=s.length())
        break;
      os<<s.substr(k,1);
    }
  return os.str();
}

void Parser::Data::eat(size_t i) throw (XMLParseError)
  {
    STACKTRACE;
    if(pos+i>s.length())
      {
        throw XMLParseError("in Parse::Data::eat - no data left");
      }
    for(size_t j=0;j<i;j++)
      if(s[pos+j]=='\n')
        line++;

    pos+=i;
  }

void Parser::Data::push()
  {
    stack.push_back(pos);
    linestack.push_back(line);
  }

void Parser::Data::pop()
  {
    pos=stack.back();
    stack.pop_back();

    line=linestack.back();
    linestack.pop_back();
  }

void Parser::Data::discard()
  {
    stack.pop_back();
    linestack.pop_back();
  }

void Parser::Data::eatBlanks() throw (XMLParseError)
  {
    AGString f=getFirst(1);

    while(f==" " || f=="\t" || f=="\n")
      {
        try{
          eat(1);
        }
        catch(XMLParseError)
          {
            throw XMLParseError("Tried to eat blanks, but data was left.");
          }
        f=getFirst(1);
      }

  }

AGString Parser::Data::getTil(const AGString &p) const
{
  size_t i=s.find(p,pos);
  if(i!=s.npos)
    return s.substr(pos,i-pos);
  return s.substr(pos,i);
}

Parser::~Parser()
  {
  }

size_t Parser::getLine() const
{
  return data.line;
}

void Parser::parse(const AGString &pData)
  {
    data.s=AGString(pData);

    parseHeader();
    while(!eod())
      {
        //      cdebug(data.pos<<":"<<data.getFirst(5));
        if(!parseComment())
          if(!parseNodeEnd())
            {
              NodeStartInfo i=parseNodeStart();
              if(!i.startTag)
                if(!parseText())
                  {
                    if(!eod())
                      {
                        cdebug("ERROR AT line:"<<getLine()<<":"<<data.getFirst(20));
                        break;
                      }
                  }
            }
      }
  }

bool Parser::parseText()
  {
    STACKTRACE;

    if(!data.getFirst(1).length())
      return false;
    //  cdebug("current:"<<data.getFirst(10));
    //  cdebug((int)data.getFirst(10)[0]<<"  "<<int(EOF));

    AGString ctext=data.getTil("<");
    //  data.eat(ctext.length());
    if(ctext.length())
      {
        data.eat(ctext.length());
        //  cdebug("text:"<<ctext);
        //      cdebug("NEXT:"<<data.getFirst(1));


        text(ctext);
        return true;
      }
    return false;


    AGStringStream os;
    while(data.getFirst(1)!="<" && data.getFirst(1).length())
      {
        os<<data.getFirst(1);
        data.eat(1);
      }
    if(os.str().length())
      {
        text(os.str());

        cdebug(os.str().length()<<"  "<<ctext.length());
        cdebug(":"<<os.str()<<"!");
        //      cdebug((int)os.str()[0]);
        assert(os.str()==ctext);
        return true;
      }
    else
      assert(ctext.length()==0);
    return false;
  }

bool Parser::parseComment()
  {
    if(data.first("<!--"))
      {
        data.eat(4);
        AGString s=data.getTil("-->");
        comment(s);
        data.eat(s.length()+3);
        return true;
      }
    return false;
  }

AGString Parser::parseName()
  {
    AGStringStream os;
    AGString first=data.getFirst(1);
    while(first!=" " && first!="\t" && first!=">" && first!="/")
      {
        os<<first;
        data.eat(1);
        first=data.getFirst(1);
      }
    return os.str();
  }

Parser::NodeStartInfo Parser::parseNodeStart()
  {
    NodeStartInfo info;
    info.startTag=false;
    if(!data.first("<"))
      return info;

    info.startTag=true;
    data.eat(1);
    AGString name=parseName();
    //  cdebug("name:"<<name);
    //  data.eat(name.length());

    Node::Attributes attrs=parseAttributes();

    //  cdebug("pos:"<<data.pos<<":"<<data.getFirst(5));
    data.eatBlanks();
    if(data.first("/"))
      {
        //      cdebug("simple");
        info.simple=true;
        data.eat(1);
      }
    else
      info.simple=false;
    assert(data.first(">"));
    data.eat(1);
    if(info.simple)
      simpleTag(name,attrs);
    else
      startTag(name,attrs);
    return info;
  }


bool Parser::parseNodeEnd()
  {
    if(!data.first("</"))
      return false;

    data.eat(2);
    AGString name=data.getTil(">");
    data.eat(name.length()+1);
    endTag(name);
    return true;
  }

bool Parser::parseHeader()
  {
    //  cdebug("header:"<<data.first("<?"));
    //  cdebug("header:"<<data.getFirst(2));
    if(!data.first("<?"))
      return false;
    data.push();
    data.eat(2);
    AGString c=data.getTil("?>");
    //  cdebug("c:"<<c);
    if(c.length()==0)
      {
        data.pop();
        return false;
      }
    data.eat(c.length());
    if(data.first("?>"))
      {
        header(c);
        data.discard();
        data.eat(2);
        return true;
      }
    data.pop();
    return false;
  }

AGString Parser::parseString()
  {
    AGString start=data.getFirst(1);
    AGString first;
    AGStringStream os;
    data.eat(1);

    bool escape, sescape;
    escape=sescape=false;

    do
      {
        first=data.getFirst(1);
        data.eat(1);
        if(escape)
          {
            escape=false;
            os<<"\\"<<first;
          }
        else if(first=="\\")
          {
            escape=true;
          }
        else
          {
            if(first==start)
              break;
            else
              os<<first;
          }

      }while(true);

    return os.str();
  }

Node::Attributes Parser::parseAttributes()
  {
    Node::Attributes attrs;
    AGString name,value;
    //  cdebug("first:"<<data.getFirst(1));

    while(!(data.first(">") || data.first("/")))
      {
        data.eatBlanks();
        name=data.getTil("=");
        data.eat(name.length()+1);

        value=parseString();

        //      cdebug("attr:"<<name<<" = "<<value);
        attrs[name]=value;
      }
    return attrs;
  }

bool Parser::eod()
  {
    //  cdebug(data.pos<<"  "<<data.s.length());
    return data.pos>=data.s.length();
  }

void Parser::simpleTag(const AGString &pName,const Node::Attributes &pAttributes)
  {
    cdebug("simple:"<<pName);
  }

void Parser::startTag(const AGString &pName,const Node::Attributes &pAttributes)
  {
    cdebug("start:"<<pName);
  }
void Parser::endTag(const AGString &pName)
  {
    cdebug("end:"<<pName);
  }
void Parser::text(const AGString &pText)
  {
    cdebug("text:"<<pText);

  }
void Parser::comment(const AGString &pText)
  {
    cdebug("comment:"<<pText);
  }



void Parser::header(const AGString &pText)
  {
    cdebug("header:"<<pText);
  }


/////////////////////////////////////////////////////////////////////////////////////
// DomParser
/////////////////////////////////////////////////////////////////////////////////////

void DomParser::simpleTag(const AGString &pName,const Node::Attributes &pAttributes)
  {
    if(nodes.size()==0)
      {
        Node &n=doc->root();

        n.setName(pName);
        n.setAttributes(pAttributes);
      }
    else
      {
        Node *p=nodes.back();
        Node &n=p->addChild(pName);
        n.setAttributes(pAttributes);
      }
  }
void DomParser::startTag(const AGString &pName,const Node::Attributes &pAttributes)
  {
    if(nodes.size()==0)
      {
        Node &n=doc->root();

        n.setName(pName);
        n.setAttributes(pAttributes);
        nodes.push_back(&n);
      }
    else
      {
        Node *p=nodes.back();
        Node &n=p->addChild(pName);
        n.setAttributes(pAttributes);
        nodes.push_back(&n);
      }
  }
void DomParser::endTag(const AGString &pName)
  {
    if(nodes.size()==0)
      {
        std::cerr<<"ERROR:close tag for '"<<pName<<"' and no start tag at all for this! line:"<<getLine()<<std::endl;
      }
    else
      {
        if(nodes.back()->getName()!=pName)
          {
            std::cerr<<"ERROR: close tag for '"<<pName<<"' found, but '"<<nodes.back()->getName()<<"' expected! line:"<<getLine()<<std::endl;
          }
        else
          {
            nodes.pop_back();
          }
      }
  }
void DomParser::text(const AGString &pText)
  {
    if(nodes.size()>0) // ignore text otherwise
      {
        nodes.back()->addChild("").setContent(pText);
      }
  }
void DomParser::comment(const AGString &pText)
  {
    nodes.back()->addChild("").setContent(AGString("<!--")+pText+AGString("-->"));
  }
void DomParser::header(const AGString &pText)
  {
    // do nothing
  }


Document *DomParser::parse(const AGString &pData,Document *d)
  {
    doc=d;
    nodes.clear();
    Parser::parse(pData);

    nodes.clear();
    return doc;
  }

Document *DomParser::parse(const AGString &pData)
  {
    return parse(pData,new Document);
  }
