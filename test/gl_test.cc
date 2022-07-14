#include <iostream>
#include "lib/ag_main.h"
#include "lib/gui/ag_application.h"
#include "lib/gui/ag_button.h"
#include "lib/ag_rand.h"
#include "lib/video/ag_video.h"

class TestApp: public AGApplication {
};


int main() {
  AGMain main;
  main.setVideo(new AGVideoManager());

  main.getVideo()->initVideo(1024, 768, 32, false, true);

  TestApp app;

  AGButton *button=new AGButton(0, AGRect2(0,0,300,100),"TEST");
  button->setCaching(false);
  app.setMainWidget(button);
  app.run();

  return 0;

}
