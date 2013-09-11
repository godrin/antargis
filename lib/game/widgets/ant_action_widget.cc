#include "ant_action_widget.h"
#include "ag_button.h"
#include "rk_debug.h"

const char *AntActionWidget::ActionImages[]=
{
  "data/gui/sword.png",
  "data/gui/take_apple.png",
  "data/gui/take_weapon.png",
  "data/gui/recruit.png",
  "data/gui/construct.png",
  "data/gui/dismiss.png",
  "data/gui/bed.png",
  "data/gui/support.png",
  "data/gui/drop_apple.png",
  "data/gui/drop_weapon.png",
  "data/gui/take_apple.png", // Harvest

};


AntActionWidget::AntActionWidget ( AGWidget* pParent, const AGRect2& r ) :AGWidget ( pParent,r )
{
}
AntActionWidget::~AntActionWidget() throw()
{
}


void AntActionWidget::setHandler ( AntActionWidget::Handler* pHandler )
{
  handler=pHandler;
}

void AntActionWidget::show ( const std::vector< AntActionWidget::Action >& actions,const AGVector2 &target )
{
  AGWidget::show();
  clear();
  float w=50;
  AGVector2 pos ( 0,0 );
  AGVector2 size ( w,w );
  AGVector2 allsize ( size );
  AGVector2 stepping ( w,0 );
  for ( std::vector<AntActionWidget::Action>::const_iterator i=actions.begin(); i!=actions.end(); i++ )
    {
      //AGButton *b=new AGButton(this,AGRect2(pos,pos+size),"TEXT");
      const char *image=ActionImages[*i];
      if ( !image )
        {
          throw std::string ( "No image found for action" );
        }
      AGButton *b=AGButton::create ( this,AGRect2 ( pos,pos+size ),"",image,true,"" );
      addChild ( b );
      actionButtons[b]=*i;
      b->sigClick.connect ( slot ( this,&AntActionWidget::eventButtonClicked ) );
      pos+=stepping;
      allsize+=stepping;
    }
  AGRect2 puh ( target,target+allsize );
  cdebug ( "puh:"<<puh );
  setRect ( puh );

  //setWidth(40);
  //setHeight(40);
}

bool AntActionWidget::eventButtonClicked ( AGEvent* m )
{
  AGListener *l=m->getCaller();
  AGButton *b=dynamic_cast<AGButton*> ( l );
  if ( b )
    {
      std::cout<<"L:"<<l<<" "<<typeid ( *l ).name() <<std::endl;
      if ( handler )
        handler->actionClicked ( actionButtons[b] );
    }
  return AGWidget::eventMouseClick ( m );
}
