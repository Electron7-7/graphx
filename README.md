##### Heads up: I forgor that stuff like image files aren't packed into the program when compiled, sooooo.... filepaths are kinda broken Unless you put the github repo folder in the same place I do (on Windows, I'm pretty sure this is just fucked up until I learn how to unfuck it, whereas on Linux I think you can get away with putting it in ur home folder)

###### Big fucking thanks to [this absolute __legend__](https://learnopengl.com) for being the only reason I didn't give up on this thing the second I ran my first compile (and got fifty linker errors).

###### Also big shoutout to [the DOOM source code on github](https://github.com/id-software/DOOM) and the [Game Engine Black Book on DOOM](https://fabiensanglard.net/gebbdoom)

###### And I cannot forget [this chad](https://github.com/sadmansk/GameEngine) for making a (at the time of writing this) very simple C++ & OpenGL game engine (just like I'm trying to do!). I referenced his repo just as often, if not more than the Quake and DOOM repos.

#### Building GraphX
Yeesh, you really wanna try? Well, if you're on Linux then all you gotta do is run good ol' `make`, and a "graphx_linux" program will appear in the build directory.
<br></br>
If you're on Windows... god help you.
<br></br>
Just kidding! I did all the painful shit for you! Kinda. You'll need to install the Visual Studio Build Tools & LLVM Clang components, as well as the "Desktop Development With C++" Workload (just the default installation should do). You'll also need to install CMake to generate the Makefiles and other stuff. Set the CMake generator to "Unix Makefiles" and make sure you choose to "Specify native compilers"; set the C compiler to clang.exe and the C++ compiler to clang++.exe. On my system, clang & clang++ are here: "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/Llvm/x64/bin". They'll probably be there for you, too, but you should always double check. Set your CMAKE_INSTALL_PREFIX accordingly, and if you don't want a .pdb file, set CMAKE_BUILD_TYPE to "Release". Click "Configure", wait, and click "Generate". Once everything's done, you can *finally* navigate to the build directory with a Command Prompt (or the Terminal app if you're fancy and cool, like me), run good ol' `make`, and (hopefully) a "graphx_windows.exe" executable will appear in the build directory. It'll probably open a Command Prompt window when you run it; that's just because I didn't tell CMake to tell the compiler to tell Windows "hey, don't, uh... don't do that". I'll probably fix that later(tm).