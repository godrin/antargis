#include "ag_config.h"
#include "ag_xml.h"
#include "ag_fs.h"
#include "rk_rubyobj.h"

// TODO: AGConfig should switch to a ruby-based configuration (maybe YAML)
// TODO: commenting should be possible !! (no overwriting)

AGConfig::AGConfig()
  {
    Document doc("config.xml");

    Node &root=doc.root();
    AGString comment;
    for(Node::iterator i=root.begin();i!=root.end();i++)
      {
        if((*i)->get("name")=="")
          {
            AGString c=(*i)->getContent();
            if(c.substr(0,4)=="<!--")
              {
                comment=c;
              }
          }
        else
          {
            singleValue[(*i)->get("name")]=(*i)->get("value");
            comments[(*i)->get("name")]=comment;
            comment="";
          }
      }
    //  writeToDisc();
  }

AGString AGConfig::get(const AGString &pValue,const AGString &pDefault,const AGString &pComment)
  {
    AGString v=get(pValue);
    if(v=="")
      {
        comments[pValue]=pComment;
        set(pValue,pDefault);
        v=pDefault;
      }
    return v;
  }



AGString AGConfig::get(const AGString &pValue) const
{
  std::map<AGString,AGString>::const_iterator i=singleValue.find(pValue);
  if(i==singleValue.end())
    return "";
  else
    return i->second;
}

void AGConfig::set(const AGString &pName,const AGString &pValue)
  {
    AGString old=singleValue[pName];
    if(old!=pValue)
      {
        singleValue[pName]=pValue;
        writeToDisc();
      }
  }

void AGConfig::writeToDisc()
  {
    Document doc;
    Node &root=doc.root();
    root.setName("config");
    for(std::map<AGString,AGString>::const_iterator i=singleValue.begin();i!=singleValue.end();i++)
      {
        if(comments[i->first]!="")
          {
            Node &n=root.addChild("");
            n.setContent(AGString("\n")+comments[i->first]+"\n");
          }

        Node &n=root.addChild("option");
        n.set("name",i->first);
        n.set("value",i->second);
      }

    saveFile("config.xml",doc.toString(true));
  }



AGConfig *gConfig=0;
AGConfig *getConfig()
  {
    if(!gConfig)
      gConfig=new AGConfig;
    return gConfig;
  }

void updateConfig()
  {
    checkedDelete(gConfig);
    gConfig=0;
    //  getConfig();
  }
