#include <iostream>
#include <ag_main.h>
#include <ag_video.h>

int main(int argc, char **argv) {
    std::cout << "Hello, world!" << std::endl;
    
    AGMain main;
    main.setVideo(new AGVideoManager());
    
    main.getVideo()->initVideo(800,600,32,false,true);
    
    return 0;
}
