#include <iostream>
#include <ag_main.h>
#include <ag_video.h>
#include <ag_application.h>
#include <ant_intro_app.h>

int main(int argc, char **argv) {
    std::cout << "Hello, world!" << std::endl;
    
    
    AGMain main;
    main.setVideo(new AGVideoManager());
    
    main.getVideo()->initVideo(800,600,32,false,true);
  
    //AGApplication app;
    AntIntroApp app;
    app.init();
    app.run();
    
    return 0;
}
