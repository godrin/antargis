Battles of Antargis
===================
Antargis on Google+: <a href="https://plus.google.com/101003433246259562872" rel="publisher">Google+</a>


Description
-----------

Battles of Antargis is a project to develop a real-time-strategy (RTS) game, which is a
mixture of 3 three games:
* Powermonger
* Settlers
* Warcraft

The main purpose of this game is conquering a medieval-like world. While doing so with your
hero(es) you meet friends and enemies. Enemies can be fought with your troopers that you can
arm with weapons and armour. In some cases you'll have to build these weapons or new buildings.

The deciding differences to normal RTS-games like Warcraft are:
* <tt>Story-bound</tt> - the game is strongly story-bound. So you will be told some story while playing the game and not only in between the different levels
* <tt>Only 2 unit-types</tt> - This game only provides 2 kinds of units: heroes and troopers. The latter are the normal people, which do their work and can be recruited by heroes. Each player has an avatar within the game. This one is the player himself. So if this avatar dies, the game (or the level) is lost. The player can however gain control over other heroes.
* <tt>Restricted control</tt> -  You can't control your troopers directly. You can assign jobs to your heroes. They will then automatically control their troopers.

Overview
--------

The game is devided into several parts, which have each a specified purpose.
The main difference between parts is the language, in that they're written, C++ or Ruby.

The main language of BoA is Ruby. But because most low-level-libraries cannot directly be used by Ruby and
for some things Ruby is just too slow, there are some parts of the Engine, that are written in C++.

The game is parted into several layers, that shouldn't be mixed too much. These layers are:
* external libraries (Ruby,SDL,libstdc++,...)
* basic-lib, which includes basic functions like filesystem-access, basic geometry-processing, event-handling and so forth
* math-lib - holding extended math-functions
* video-lib - providing a (nearly) seamless usage of either OpenGL or plain (2D-) SDL-Mode for video-access
* sound-lib - providing some simplified access to SDL_Mixer
* gui-lib - a small gui-lib, that's pretty easy to use and designed for usage in games
* 3dengine - a pretty small 3d-engine, that can be used for RTS-games (it's not designed to work for FPS or the like)
* game-lib - basic functionality for RTS games. It includes those functions that are called frequently and thus must be pretty fast.

The layers above are all done in C++. What now follows are the Ruby-parts:
* much glue-code in ruby for menus, dialogs and the like 
* game-logic

1. link:files/ext/gui/README.html - information about Antargis GUI-lib
2. link:files/ext/game/README.html - Information about Antargis game-engine
3. link:files/ruby/README.html - Game logic, esp. Multiplayer, AI- and level-scripting, dialogs, menus...

== Installation and running
See INSTALL

== Make dist / Packages
mkdir build
cd build
cmake ..
make -j4 package
