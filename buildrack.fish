#!/usr/bin/env fish
set -x VERSION 2.6.2 #<--- Set me to the appropriate version!
set_color brcyan; echo Rack version set to: $VERSION
set_color bryellow; echo Building Rack...
make -j4 #<--- Use as many or as few threads as you want, can, or like.
set_color brgreen; echo Done!
set_color normal;