<a href="https://github.com/Electron7-7/graphx"><img src="https://github.com/user-attachments/assets/33d7c8db-ad36-4fbb-ae90-fd94b7afa180" width="150"/></a>
<a href="https://github.com/Electron7-7/graphx"><img src="https://github.com/user-attachments/assets/4ca82421-9f0d-41d9-a705-7aeb2986dd18" width="150"/></a>
<a href="https://github.com/Electron7-7/graphx"><img src="https://github.com/user-attachments/assets/6adf09d2-67e6-4467-8ae9-2afbcd57ac6e" width="150"/></a>
  <p>Logo design: <a href="https://www.instagram.com/m0in126">Moin</a></p>
<div id="user-content-toc">
  <ul align="center" style="list-style: none;">
    <summary>
      <h1>GraphX</h1>&nbsp;&nbsp;&nbsp;&nbsp;<h3>(Or, "How I stopped worrying and learned to love C++")</h3>
    </summary>
  </ul>
</div>

<img src="https://github.com/user-attachments/assets/7b795eb4-574c-4760-87ba-497b336df9db" width="400"/>
<img src="https://github.com/user-attachments/assets/6afd76e4-0f0e-48d6-862b-973da095fa21" width="400"/>

## Update 3: GT Files!

No, not Gran Turismo, "GraphXTheatre" files are the newest addition to the GraphX game engine, and I am thoroughly proud of my horrible code. In a nutshell, I decided to create my own custom file format for creating Theatres and I wanted to do so without using any tutorials or guides (just StackOverflow and tenacity). It was very painful, but very informative! I created my own lexer, parser, and interpreter for a file format/syntax that I also invented, again, without any guides or even learning what the difference between a lexer and a parser is until after I had finished (i.e: right now as I'm writing this, lmfao). If you want an in-depth guide to this new file format, check out the [GraphX Theatre Syntax Documentation](https://github.com/Electron7-7/graphx/blob/trunk/src/theatres/docs/graphxtheatre_syntax_documentation.md).

## An Okay Game Engine

When I started this journey, I had very little programming experience; a few basic Python programs (like, command-line output basic), some shell scripting, a bit of web-design, and some fun ventures in [Blitz3D](https://github.com/blitz-research/blitz3d) (which I highly recommend to anyone interested in the programming side of gamedev). Needless to say, I was in no position _whatsoever_ to think to myself, "hmm, you know what? I want to learn C++ _and_ OpenGL/Graphics Programming at the same exact time! Surely, nothing will go wrong". I am here to tell you I am lucky that I have autism and an autistic intrigue and passion for programming, because if I didn't, this project would have permanently died before a week's time.
**DO NOT DO WHAT I DID**.
If you want to learn something, especially learn how to _make_ something, don't fucking jump into the deep end. You'll burn out so quickly that you'll never want to touch that thing ever again. I know this because I did just that with Blender, Unity, UE5, programming, video creation, video editing, working out, voice acting, animation, drawing, and so much more. It took **_years_** for me to pick some of those things back up, and some of them I _still_ haven't tried since I burnt out on them. I am lucky that game engine programming just so happened to be an autistic hyperfixation of mine. That's quite literally the only thing that kept me going while essentially learning the most difficult three things you can ever learn: coding, C++, and graphics programming. Thank god I didn't try this on Windows.
Oh yeah, my final word of advice:
**_DO NOT USE WINDOWS TO LEARN HOW TO PROGRAM!!!!!!_**
Holy **fuck** is Windows just the worst for beginners; Visual Studio, the Windows libraries, and shit like VisualC++ are all fucking _awful_ for beginners. When you have to learn how to fucking open up and configure Visual Studio before even writing "Hello World!", you know there's a problem. Fuck Visual Studio, fuck solution files, fuck what it does to your beautiful project folder, fuck Microsoft and their stupid unique required libraries for executable compilation, and fuck the NT filesystem for existing.
Rant over, you can all return to your desks.
## Building GraphX
#### A Note About The Makefile
I designed the Makefile to use unique variables and build rules when targeting either Windows or Linux, in an effort to isolate both architectures. When building for Linux, the Makefile will use the variables `CXX`, `CC`, `CXXFLAGS`, `CFLAGS`, `INCLUDES`, etc. When building for Windows, however, the Makefile uses the variables `WCXX`, `WCC`, `WCXXFLAGS`, `WCFLAGS`, `WINCLUDES`, etc.
My reasoning is that I wanted it to be easy for people to swap out compilers, linker flags, include locations, etc for either architecture while keeping them both separated. It feels neater and a little more modular like this (although it's probably less efficient). The way I handle embedding resources is less elegant and requires you to manually add each image/shader to an image/shader list, but I intend on changing that as soon as I've got resource embedding working on Windows, using `xxd.exe`.
#### Building for Linux
To compile GraphX for Linux, you'll need:
- `GLFW`
- `clang` & `clang++`
- `make`

If you don't want to use clang for whatever reason, you can change the `CXX` and `CC` variables in the Makefile. I've kept all the variables that people would most likely want/need to change at the top of the Makefile for ease of access. GraphX also relies on some headers that are only included in C++20, so don't remove the `-std=c++20` from `CXXFLAGS` or `WCXXFLAGS`. If you're getting compiler errors related to `std::lerp`, it's probably because you're not telling the compiler to enable C++20; the way I do it is by using the `-std=c++20` flag in the Makefile.

To build GraphX, just run `make` or `make build` from the root directory (the folder with the Makefile in it). If everything finishes successfully, the program will be inside the pre-existing `build/` directory. Its name will be different depending on your distro, but it will always start with `GraphX_Linux`.
#### Building for Windows (on Linux)
To compile GraphX for Windows while on Linux, you'll need:
- `mingw-w64-gcc`
- `mingw-w64-g++`
- `make`

I've included all the libraries and header files needed to compile an executable on Linux using MinGW, and the Makefile is already configured to handle everything correctly, so all you need to do is run `make windows` from the root directory, and if it all finishes successfully, you'll find `Graphx_Windows_x86_64.exe` in the pre-existing `build/` directory.
#### Building for Windows (on Windows)
### Building GraphX on Windows is not currently supported! The reason: WINDOWS SUCKS!! The slightly longer reason: I refuse to be forced to use VisualStudio and all the things I've added to GraphX with this update have broken how I used to compile it on Windows.
### I will keep working on getting native Windows compilation working, don't worry, but for now you'll have to settle with using MinGW or WSL (or do my work for me which would be super cool)
### The methods described below will (most likely) not work, but I'm keeping them in the README because I'm too lazy to remove them and I do intend on getting native Windows compilation working again very soon.
(temporary note: currently, I have just finished implementing embedded resources, using the Linux tool `xxd`. I've included a Windows version of the tool in `src/windows_dependencies/xxd.exe` but have yet to update the CMakeLists.txt to correctly embed resources. Once I do, this note will be removed)

To compile GraphX for Windows on Windows you'll need:
- `the grace of God`
- `an exorcist or Catholic priest`
- `fifteen "Hail Marys"`
- `Microsoft Build Tools`
- `CMake` (this is what the exorcist/priest is for)
- ~~`Visual Studio`~~ HAHA! IN YOUR FACE, MICROSOFT!

Previously, I was able to get GraphX compiling natively on Windows using CMake and some black magic. I have a personal vendetta against Visual Studio, so I chose to do everything I could to not let it sully my project.

However, this has come at a cost: Windows Jank<sup>tm</sup><br>

Here's the way I use `CMake` to compile GraphX:
1. I install and choose `clang` and `clang++` as my native compilers.
2. I set the output to "Unix Makefiles".
3. I set `CMAKE_BUILD_TYPE` to `"Release"`, otherwise you'll get some debugging files that you probably don't care about.
4. I press `configure`
5. I pray
6. If all goes well, I press `generate`
7. I pray, again
8. If all goes well, I open Windows Terminal (or Command Line) in the `build/` directory and run `make`
9. If successfull, `graphx_windows_x86_64.exe` should now appear in the `build/` directory
10. If any of these steps failed... that's what the exorcist/priest is for

I really hate compiling C++ programs on Windows (I hate a lot about Windows, to be quite frank), but I'll keep trying to make it simpler and easier, so hopefully this part of the README becomes outdated.
## Special Thanks
Big shout out to [LearnOpenGL](https://learnopengl.com), [StackOverflow](https://stackoverflow.com), and [Desmos](https://desmos.com) for teaching me everything I need to know.

Huge thank you to the [Game Engine Black Book on DOOM](https://fabiensanglard.net/gebbdoom), and autistic thank you to [John Carmack for programming DOOM](https://github.com/id-software/DOOM).

Kudos to [sadmansk and his game engine](https://github.com/sadmansk/GameEngine) for being a helpful reference during the first few days of this project.

Thank you to UE5 and Unity for making me annoyed, passionate, and foolish enough to say "this sucks, I dislike this, and I could do better".

Thank you to [Godot](https://github.com/godotengine/godot) for being a potentially helpful reference once I understand more about what the hell I'm doing, but mainly for being open-source, well made (mostly), and having [a clear and concise chart detailing their graphics rendering pipeline](https://docs.godotengine.org/en/latest/_images/rendering_architecture_diagram.webp).

Finally, but most importantly, thank you to my dad for giving me his love of problem solving, a passion for knowledge, and "Algorithms in C: Third Edition" which is a very good read (that I have yet to finish).
