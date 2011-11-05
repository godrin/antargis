#include <ag_button.h>
#include <ag_checkbox.h>
#include <ag_colorbutton.h>
#include <ag_combo.h>
#include <ag_edit.h>
#include <ag_frame.h>
#include <ag_image.h>
#include <ag_layout.h>
#include <ag_listbox.h>
#include <ag_local.h>
#include <ag_radio.h>
#include <ag_radiogroup.h>
#include <ag_table.h>
#include <ag_theme.h>
#include <ag_window.h>
#include <ag_screenwidget.h>
#include <ag_cell.h>
#include <ag_scrollingwidget.h>

#include "ag_layoutfactory.h"

AGWidget *parseNode(AGWidget *pParent,const Node &pNode);


// Layout-Factories
class AGButtonLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Button,"button")
  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGButton *b;
      AGStringUtf8 caption=_(pNode.get("caption"));
      b=new AGButton(pParent,pRect,AGStringUtf8(caption));
      setResult(b);
      AGString captionImage=pNode.get("caption-image");
      if(captionImage.length())
        b->setSurface(AGSurface::load(captionImage),false);
      if(pNode.get("enabled")=="false")
        b->setEnabled(false);

      b->setTheme(pNode.get("theme"));
    }
};


class AGTableLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Table,"table")


  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGWidget *w=parseTable(pParent,pNode,pRect);
      pParent->addChild(w);
      w=0;
      setResult(0);
    }

  AGTable *parseTable(AGWidget *pParent,const Node &pNode,const AGRect2 &geom)
    {
      AGTable *t;
      t=new AGTable(pParent,geom);
      setResult(t); // set result - so garbage collector gets to know this one

      int w,h;
      w=pNode.get("cols").toInt();
      h=pNode.get("rows").toInt();

      t->setName(pNode.get("name"));

      if(pNode.get("visible")=="false")
        t->hide();


      std::vector<std::pair<float,bool> > rows(h);
      std::vector<std::pair<float,bool> > cols(w);

      // parse rows/cols info
      Node::const_iterator i=pNode.begin();
      for(;i!=pNode.end();i++)
        {
          if((*i)->getName()=="colsize")
            {
              int id=(*i)->get("col").toInt();
              assert(id>=0 && id<w);
              AGString s=(*i)->get("fixed");
              if(s.length())
                cols[id]=std::make_pair(s.toFloat(),true);
              else
                {
                  s=(*i)->get("relative");
                  cols[id]=std::make_pair(s.toFloat(),false);
                }
            }
          else if((*i)->getName()=="rowsize")
            {
              int id=(*i)->get("row").toInt();
              assert(id>=0 && id<h);

              AGString s=(*i)->get("fixed");
              if(s.length())
                rows[id]=std::make_pair(s.toFloat(),true);
              else
                {
                  s=(*i)->get("relative");
                  rows[id]=std::make_pair(s.toFloat(),false);
                }
            }
        }

      for(int k=0;k<w;k++)
        {
          if(cols[k].second)
            t->addFixedColumn(cols[k].first);
          else if(cols[k].first==0.0f) // not inited
            t->addColumn(1.0f);
          else
            t->addColumn(cols[k].first);
        }
      for(int k=0;k<h;k++)
        {
          if(rows[k].second)
            t->addFixedRow(rows[k].first);
          else if(rows[k].first==0.0f) // not inited
            t->addRow(1.0f);
          else
            t->addRow(rows[k].first);
        }

      // now get children

      i=pNode.begin();
      for(;i!=pNode.end();i++)
        {
          AGWidget *w=parseNode(t,**i);
          if(w)
            {
              int col=(*i)->get("col").toInt();
              int row=(*i)->get("row").toInt();
              t->addChild(col,row,w);
            }
        }
      //  t->arrange();

      if(pNode.get("cache")=="true")
        t->setCaching(true);

      return t;
    }


};

class AGWindowLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Window,"window")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGStringUtf8 title=_(pNode.get("title"));
      AGString theme=pNode.get("theme");

      AGWidget *w=new AGWindow(pParent,pRect,AGStringUtf8(title),theme);
      setResult(w);
    }
};

IMPLEMENT_COMPONENT_FACTORY(Table);
IMPLEMENT_COMPONENT_FACTORY(Button);
IMPLEMENT_COMPONENT_FACTORY(Window);


// AGText creator
class AGTextLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Text,"text")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      //    AGWidget *w=new AGText(pParent,pRect,text,font);
      AGEdit *w=new AGEdit(pParent,pRect);
      setResult(w);


      AGStringUtf8 text=_(pNode.get("caption"));
      text=text.replace("\\n","\n");
      w->setText(AGStringUtf8(text));
      AGString fontname=pNode.get("font");
      if(fontname=="")
        fontname="text.font";

      AGFont font;
      font=getTheme()->getFont(fontname);
      w->setFont(font);

      if(pNode.get("align")=="left")
        w->setAlign(EDIT_LEFT);
      if(pNode.get("align")=="right")
        w->setAlign(EDIT_RIGHT);
      if(pNode.get("align")=="center")
        w->setAlign(EDIT_CENTER);

      if(pNode.get("valign")=="top")
        w->setVAlign(EDIT_TOP);
      if(pNode.get("valign")=="bottom")
        w->setVAlign(EDIT_BOTTOM);
      if(pNode.get("valign")=="center")
        w->setVAlign(EDIT_VCENTER);

      //    w->setAlign(EDIT_CENTER);
      w->setMutable(false);
      w->setBackground(false);
    }
};
IMPLEMENT_COMPONENT_FACTORY(Text);


// AGText creator
class AGEditLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Edit,"edit")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      CTRACE;
      AGString text=pNode.get("text");
      bool multi=pNode.get("multi")=="true";

      //    AGWidget *w=new AGText(pParent,pRect,text,font);
      AGEdit *w=new AGEdit(pParent,pRect);
      setResult(w);
      w->setText(AGStringUtf8(text));
      if(pNode.get("font")!="")
        {
          AGFont font;
          font=getTheme()->getFont(pNode.get("font"));
          w->setFont(font);
        }
      //    w->setAlign(EDIT_CENTER);
      w->setMutable(true);//false);
      w->setBackground(true);//false);
      w->setMulti(multi);
      if(!multi)
        w->setVAlign(EDIT_VCENTER);
    }
};
IMPLEMENT_COMPONENT_FACTORY(Edit);



// AGListBox creator
class AGListBoxLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(ListBox,"listBox")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGListBox *l=new AGListBox(pParent,pRect);
      if(pNode.get("theme").length()>0)
        l->setTheme(pNode.get("theme"));
      setResult(l);
    }
};
IMPLEMENT_COMPONENT_FACTORY(ListBox);


// AGComboBox creator
class AGComboBoxLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(ComboBox,"comboBox")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      CTRACE;
      AGComboBox *l=new AGComboBox(pParent,pRect);
      setResult(l);

      Node::const_iterator i=pNode.begin();
      for(;i!=pNode.end();i++)
        {
          if((*i)->getName()=="item")
            l->insertItem((*i)->get("id"),AGStringUtf8((*i)->get("text")));
        }

    }
};
IMPLEMENT_COMPONENT_FACTORY(ComboBox);



// AGLayout creator
class AGLayoutLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Layout,"layout")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      CTRACE;
      AGString filename=pNode.get("filename");
      AGWidget *w=new AGWidget(pParent,pRect);
      setResult(w);
      AGLayout *l=new AGLayout(w);
      l->loadXML(loadFile(filename));//pRect);
      w->addChild(l);
    }
};
IMPLEMENT_COMPONENT_FACTORY(Layout);


// AGLayout creator
class AGImageLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Image,"image")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGString filename=pNode.get("filename");

      AGSurface s(0,0);
      if(filename.length())
        s=AGSurface::load(filename);
      else if(pNode.get("color").length()>0)
        {
          AGSurface f(1,1);
          f.putPixel(0,0,AGColor(pNode.get("color")));
          s=f;
        }
      
      

      AGRect2 r=pRect;
      if(pNode.get("x").length())
        r.setX(pNode.get("x").toFloat());
      if(pNode.get("y").length())
        r.setY(pNode.get("y").toFloat());


      // FIXME: add subsurfaces


      AGImage *w;
      

      if(pNode.get("tile")=="true")
        w=new AGImage(pParent,r,s,true);
      else
        w=new AGImage(pParent,r,s,false);
      
      w->setScale(pNode.get("scale")=="true");

      setResult(w);
    }
};
IMPLEMENT_COMPONENT_FACTORY(Image);


// AGLayout creator
class AGFrameLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Frame,"frame")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      CTRACE;
      AGString border=pNode.get("border");
      size_t width=pNode.get("width").toInt();
      
      int height=-1;
      if(pNode.get("height").length()>0)
        height=pNode.get("height").toInt();

      AGFrame *w;
      if(border.length())
        w=new AGFrame(pParent,pRect,AGBorder(border));
      else
        w=new AGFrame(pParent,pRect,width,height);

      setResult(w);
      if(pNode.get("background").length())
        w->setBackground(AGBackground(pNode.get("background")));
    }
};
IMPLEMENT_COMPONENT_FACTORY(Frame);


// AGLayout creator
class AGCellLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Cell,"cell")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGCell *cell=new AGCell(pParent,pRect);
      if(pNode.get("padding")!="")
        {
          int padx=pNode.get("padding").toInt();
          int pady=pNode.get("padding").toInt();
          AGFrame *frame=new AGFrame(cell,cell->getRect().origin(),padx,pady);
          cell->addChild(frame);
          setClient(frame);
        }
      
      setResult(cell);
    }
};
IMPLEMENT_COMPONENT_FACTORY(Cell);


// AGRadio creator
class AGCheckBoxLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(CheckBox,"checkBox")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGCheckBox *b=new AGCheckBox(pParent,pRect);
      AGStringUtf8 caption=_(pNode.get("caption"));
      if(caption.length())
        b->setCaption(AGStringUtf8(caption));

      std::string captionImage=pNode.get("caption-image");
      if(captionImage.length())
        b->setSurface(AGSurface::load(captionImage),false);
      if(pNode.get("enabled")=="false")
        b->setEnabled(false);
      if(pNode.get("theme").length())
        b->setTheme(pNode.get("theme"));
      if(pNode.get("checked")=="true")
        b->setChecked(true);

      if(pNode.get("disabledImage")!="" && pNode.get("enabledImage")!="")
        {
          b->setSurfaces(AGSurface::load(pNode.get("disabledImage")),AGSurface::load(pNode.get("enabledImage")));
        }


      setResult(b);
    }
};
IMPLEMENT_COMPONENT_FACTORY(CheckBox);

// Layout-Factories
class AGColorButtonLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(ColorButton,"colorButton")
  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      setResult(new AGColorButton(pParent,pRect,pNode.get("gridx").toInt(),pNode.get("gridy").toInt()));
    }
};
IMPLEMENT_COMPONENT_FACTORY(ColorButton);



// AGRadioGroup creator
class AGRadioGroupLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(RadioGroup,"radioGroup")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      setResult(new AGRadioGroup(pParent,pRect));
    }
};
IMPLEMENT_COMPONENT_FACTORY(RadioGroup);

// AGRadio creator
class AGRadioLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(Radio,"radio")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      AGRadio *b=new AGRadio(pParent,pRect);
      AGStringUtf8 caption=_(pNode.get("caption"));
      if(caption.length())
        b->setCaption(AGStringUtf8(caption));

      AGString captionImage=pNode.get("caption-image");
      if(captionImage.length())
        b->setSurface(AGSurface::load(captionImage),false);
      if(pNode.get("enabled")=="false")
        b->setEnabled(false);
      if(pNode.get("theme").length())
        b->setTheme(pNode.get("theme"));
      if(pNode.get("checked")=="true")
        b->setChecked(true);
      setResult(b);
    }
};
IMPLEMENT_COMPONENT_FACTORY(Radio);



class AGScreenWidgetLayoutCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(ScreenWidget,"screenWidget")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      CTRACE;
      setResult(new AGScreenWidget());
    }
};
IMPLEMENT_COMPONENT_FACTORY(ScreenWidget);




class AGScrollingWidgetCreator:public AGLayoutCreator
{
public:
  REGISTER_COMPONENT(AGScrollingWidget,"scrollingWidget")

  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
    {
      CTRACE;
      AGScrollingWidget *w;
      setResult(w=new AGScrollingWidget(pParent,pRect));
      AGRect2 r=pRect.origin();
      AGString s=(pNode.get("clientRect"));
      if(s.length()>0)
        {
          r=AGRect2(s);
        }
      w->setClientRect(r);
    }
};
IMPLEMENT_COMPONENT_FACTORY(ScrollingWidget);


void AGLayout::registerLayouts()
  {
    getLayoutFactory()->addCreator("checkBox",new AGCheckBoxLayoutCreator);
    getLayoutFactory()->addCreator("colorButton",new AGColorButtonLayoutCreator);
    getLayoutFactory()->addCreator("table",new AGTableLayoutCreator);
    getLayoutFactory()->addCreator("button",new AGButtonLayoutCreator);
    getLayoutFactory()->addCreator("window",new AGWindowLayoutCreator);
    getLayoutFactory()->addCreator("text",new AGTextLayoutCreator);
    getLayoutFactory()->addCreator("edit",new AGEditLayoutCreator);
    getLayoutFactory()->addCreator("listBox",new AGListBoxLayoutCreator);
    getLayoutFactory()->addCreator("comboBox",new AGComboBoxLayoutCreator);
    getLayoutFactory()->addCreator("layout",new AGLayoutLayoutCreator);
    getLayoutFactory()->addCreator("image",new AGImageLayoutCreator);
    getLayoutFactory()->addCreator("frame",new AGFrameLayoutCreator);
    getLayoutFactory()->addCreator("cell",new AGCellLayoutCreator);
    getLayoutFactory()->addCreator("radioGroup",new AGRadioGroupLayoutCreator);
    getLayoutFactory()->addCreator("radio",new AGRadioLayoutCreator);
    //  getLayoutFactory()->addCreator("miniMap",new AGMiniMapLayoutCreator);
    getLayoutFactory()->addCreator("screenWidget",new AGScreenWidgetLayoutCreator);
    getLayoutFactory()->addCreator("scrollingWidget", new AGScrollingWidgetCreator);

  }
