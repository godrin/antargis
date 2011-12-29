#include <iostream>
#include <ag_main.h>
#include <ag_video.h>
#include <ag_application.h>
#include <ant_intro_app.h>
#include <ant_menu_app.h>
#include <ag_layout.h>
#include <ant_game_app.h>
#include <ag_rand.h>

int main ( int argc, char **argv ) {
    std::cout << "Hello, world!" << std::endl;


    AGMain main;
    main.setVideo ( new AGVideoManager() );
    main.setRand(new AGRandomizer("abc"));

    main.getVideo()->initVideo ( 1024,768,32,false,true );
    AGLayout::registerLayouts();

    if ( true ) {

        AntGameApp app ( getVideo()->width(),getVideo()->height() );
        app.init("data/levels/tutorial/tutorial0.antlvl");
        app.run();
        return 0;
    }

    AntIntroApp intro;
    intro.init();
    intro.run();

    AntMenuApp menu;
    menu.init();
    menu.run();

    return 0;
}
