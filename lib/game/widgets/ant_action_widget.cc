#include "ant_action_widget.h"
#include "ag_button.h"
#include "rk_debug.h"

AntActionWidget::AntActionWidget(AGWidget* pParent, const AGRect2& r):AGWidget(pParent,r)
{

}
AntActionWidget::~AntActionWidget() throw()
{

}


void AntActionWidget::setHandler(AntActionWidget::Handler* pHandler)
{
    handler=pHandler;
}

void AntActionWidget::show(const std::vector< AntActionWidget::Action >& actions,const AGVector2 &target)
{
    clear();
    AGVector2 pos(0,0);
    AGVector2 size(40,40);
    for (std::vector<AntActionWidget::Action>::const_iterator i=actions.begin();i!=actions.end();i++) {

        AGButton *b=new AGButton(this,AGRect2(pos,pos+size),"TEXT");
        addChild(b);
    }
    AGRect2 puh(target,target+size);
    cdebug("puh:"<<puh);
    setRect(puh);
    
    //setWidth(40);
    //setHeight(40);
}

