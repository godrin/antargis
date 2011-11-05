#include <iostream>
#include <ag_main.h>
#include <ag_video.h>
#include <ag_application.h>
#include <ant_intro_app.h>
#include <ant_menu_app.h>
#include <ag_layout.h>

int main(int argc, char **argv) {
    std::cout << "Hello, world!" << std::endl;

    
    AGMain main;
    main.setVideo(new AGVideoManager());
    
    main.getVideo()->initVideo(1024,768,32,false,true);
    AGLayout::registerLayouts();
  
    //AGApplication app;
    AntMenuApp app;
    app.init();
    app.run();
    
    return 0;
}
