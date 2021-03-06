----------------------------------------------------------------------------------

    Stunt Rally - game based on VDrift and OGRE, with Track Editor
    Copyright (C) 2013  Crystal Hammer and contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    The license GPL 3.0 applies only to code written by us.
    Which is in source dir, subdirs: editor, network, ogre and road.
    Libraries used have their own licenses.
    For data licenses look in various .txt files in data subdirs.
----------------------------------------------------------------------------------

Links

Project's Homepage:  (all links, Wiki pages)
        http://code.google.com/p/vdrift-ogre/

Releases:  (Windows installer, Linux binary)
        https://sourceforge.net/projects/stuntrally/files/
Git repositories:  (for latest sources, data, and tracks)
        https://github.com/stuntrally/
Forums:  (Report bugs, issues, ideas, comments, etc):
        http://forum.freegamedev.net/viewforum.php?f=77

Screenshots gallery:  (from all versions)
        http://picasaweb.google.com/CryHam/
Videos:  (gameplay and editor)
        http://www.youtube.com/user/TheCrystalHammer
Editor tutorial:
        http://code.google.com/p/vdrift-ogre/wiki/TrackEditor

----------------------------------------------------------------------------------

Description

The game features 127 tracks in 24 sceneries and 19 cars.
Game modes include: Single Race, Tutorials and Championships, Multiplayer,
Split Screen. Also Replays and Ghost drive are present.

The Track Editor allows modifying and creating tracks.
Both run on Linux and Windows.

The game aims at a rally style of driving (like in Richard Burns Rally),
with possible stunt elements (loops, jumps).
It also introduces road pipes, and provides a 3D spline generated road.

For full features list check: http://code.google.com/p/vdrift-ogre/wiki/Features

----------------------------------------------------------------------------------

Hardware requirements

The minimum hardware is:
a CPU with 2 cores, and GPU: GeForce 9600 GT or Radeon HD 3870,
with Shader Model 3.0 supported and 256 MB GPU RAM.

It is possible to run on older, but
integrated GPUs (or laptops) can't handle the game well (or at all).

----------------------------------------------------------------------------------

Running

In game Esc/Tab key shows/hides Options.

At first (in Options window) go to tab [Screen] and adjust resolution.
On [Graphics] tab pick settings preset according to your GPU.
Restart after changing preset, changing it in game is not recommended.

Keys used in game can be seen in Options tab [Input].
If you want to reassign keys, or have a game controller
go to tab [Input] to configure it and test range.

Change cameras with C/X (with shift for main cameras only).
Cameras can be adjusted in game, by mouse - move mouse to see actions.

All settings and logs are saved to user folder (check Wiki page [Paths]).
If you have problems running, read Wiki page [Running].
If it doesn't help, report on IRC or make an issue on Forum [SR Bugs & Help]
(if a similar topic doesn't exist). Be sure to attach your log files.

Editor

In editor, F1 key shows/hides Options,
press Ctrl-F1 to read what can be edited and how.
Tab key switches between Camera/Edit mode.
There isn't (and won't be) an undo function -
hit F5 to reload last track state, and F4 to save it.
After saving, track can be played in game (exports all data).

There is an Editor Tutorial Wiki page (link at top)
with videos: 10 chapters, 44 minutes total.

Have fun !

----------------------------------------------------------------------------------

Compiling

For newest sources and how to compile them check
project's Wiki page [Compiling].

We are using CMake and C++.
Car simulation is done by VDrift also using bullet. Rendering by OGRE.
Trees/grass by PagedGeometry, Gui by MyGUI, materials managed by shiny.

-- Currently these are used (and need to build before game):
-- newer versions can be used
OGRE 1.8 with plugins
MyGUI 3.2
Boost 1.49

SDL2-2.0.0
libvorbis-1.3.3, libogg-1.3.0
ENet 1.3.4

-- These are included in Source (and compiled with project):
Bullet 2.79 (need this version)
BtOgre *
OICS *
PagedGeometry 1.1.1 *
shiny
TinyXML
* are modified sources

----------------------------------------------------------------------------------

Help is welcomed

Currently there is 1 (or 2) active programmers.
We are looking for skilled and motivated people
that can help improving the game or editor.

Testers
    If you are able to build from sources,
    you can test and report bugs or suggestions.

Programmers
    If you know C++, OGRE or shaders, and could
    handle one (or some) of the project's [Issues], 
    or want to do something better and know how to.

Artists
    Help exporting existing models from Blender to Ogre.
    Editing meshes for game, uv unwrap, texturing etc.
    If you can create/edit 3D models
    (cars, trees, plants, rocks, objects etc),
    textures for them or for terrain.
    If you can record/edit sounds for the game.

Translators
    If you want to help translating into
    a language, check Wiki page [Localization].

Track creators
    If you manage to create an interesting track,
    it could be released in future versions.

If so, tell us on IRC at #stuntrally on freenode,
or by posting on Forum,
or email me at: cryham (at) g m a i l (dot) com.
