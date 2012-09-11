#ifndef __ANT_ACTION_WIDGET_H
#define __ANT_ACTION_WIDGET_H

#include "ag_widget.h"

#include <map>

class AntActionWidget:public AGWidget
{
public:
  enum Action {FIGHT,TAKE_FOOD,TAKE_WEAPONS,RECRUIT,INVENT,DISMISS,REST,SUPPORT,DROP_FOOD,DROP_WEAPONS,HARVEST};
  static const char *ActionImages[];
  /*

  										<button  col="0" row="0" name="doRecruit" caption-image="data/gui/recruit.png" tooltip="Recruit"/>
  							<button  col="0" row="0" name="doSupport" caption-image="data/gui/support.png" tooltip="Support in defense"/>
  							<button  col="1" row="0" name="doTakeFood" caption-image="data/gui/take_apple.png" tooltip="Take food"/>
  							<button  col="2" row="0" name="doTakeWeapons" caption-image="data/gui/take_weapon.png" tooltip="Take weapons"/>
  							<button  col="3" row="0" name="doUpgrade" caption-image="data/gui/upgrade.png" tooltip="Upgrade" enabled="false"/>
  							<button  col="3" row="0" name="doConstruct" caption-image="data/gui/construct.png" tooltip="Construct"/>
  							<button  col="3" row="0" name="doContinue" caption-image="data/gui/build.png" tooltip="Continue"/>
  							<button  col="4" row="0" name="doInspect" caption-image="data/gui/zoom.png" tooltip="Inspect object"/>

  								<button  col="0" row="0" name="doRest" caption-image="data/gui/bed.png" tooltip="Rest"/>
  					<button  col="1" row="0" name="doDismiss" caption-image="data/gui/dismiss.png" tooltip="Dismiss"/>
  					<button  col="0" row="1" name="doDropFood" caption-image="data/gui/drop_apple.png" tooltip="Drop food"/>
  					<button  col="1" row="1" name="doDropWeapon" caption-image="data/gui/drop_weapon.png" tooltip="Drop weapons"/>
  					<radio  col="1" row="2" name="doBuild" caption-image="data/gui/build.png" tooltip="Build" enabled="true"/>
  */

  class Handler
  {
  public:
    virtual void actionClicked ( Action a ) =0;
  };


  AntActionWidget ( AGWidget *pParent,const AGRect2 &r );
  virtual ~AntActionWidget() throw();

  void setHandler ( Handler *pHandler );

  void show ( const std::vector<Action> &actions,const AGVector2 &target );
  bool eventButtonClicked ( AGEvent *m );


private:
  std::map<AGWidget*,Action> actionButtons;
  Handler *handler;
};

#endif
