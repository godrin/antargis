#ifndef __ANT_GAME_APP_H
#define __ANT_GAME_APP_H

#include "gl_app.h"

class AntMap;
class AntHero;

class AntGameApp:public GLApp {
    public:
        AntGameApp ( int w, int h );
        void init();

        virtual bool eventFrame ( float pTime );
        
        AntHero *getPlayerHero();
    private:
        AntMap *mMap;
        AGLayout *layout;
};

#endif
