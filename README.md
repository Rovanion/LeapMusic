Leap Music
==========

A musical instrument built using the Leap Motion v2.


Installation
============
Download the source code of the project with git:
```bash
git clone <repo url>
cd leapMusic
```

Download the Leap SDK and copy or link the folder named LeapSDK into the project directory:

```bash
ln -s ~/Path/To/SDK/LeapSDK ./

```

Now you're ready to build the project:

``` bash
cd build
cmake ..
make
```

If you don't want to compile the project with make you can specify the type of project files CMake should generate with the -G flag. Use ```cmake --help``` to see which generators are available.


Interaction
===========
1. Open and closed hand to activate the menu.
2. Absolute position of the palm of the hand. This is the position used to modify the tone and also to navigate menus.
3. The positioning of the fingers. Pressing the index finger against the thumb records a tone.
