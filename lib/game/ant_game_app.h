#ifndef __ANT_GAME_APP_H
#define __ANT_GAME_APP_H

#include "ant_basic_game_app.h"
#include "ant_action_widget.h"

class AntMap;
class AntHero;
class AntEntity;
class AntActionWidget;

class AntGameApp:public AntBasicGameApp,public AntActionWidget::Handler {
  public:
    AntGameApp ( int w, int h );
    void init(const std::string &level);

    virtual bool eventFrame ( float pTime );

    AntHero *getCurrentHero();
    AntEntity *getSelectedEntity();
  protected:
    void eventMapClicked(const AGVector2 &pos, int button);
    void eventEntitiesClicked(const PickResult &pNodes, int button);
    void selectEntity(AntEntity *e);
    AntMap *getMap();
    void actionClicked(AntActionWidget::Action a);
  private:

    void printOutEntityInfo(AntEntity *e);

    void resetJob();

    int selectedEntityId;
    AntMap *mMap;
    AGLayout *layout;
    int currentHeroId;
    AntActionWidget *actionWidget;
    bool mPaused;
};

#endif
