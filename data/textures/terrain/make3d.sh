#!/bin/bash

# old script to generate a 3d terrain-texture
# have a look at makeTerrain.rb !

files="terrain_tile*.png"

montage +frame +shadow -geometry 128x128+0+0 -tile 1x8 $files new3d.png
