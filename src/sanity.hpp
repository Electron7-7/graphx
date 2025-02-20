// sanity.hpp - Various pre-processor includes for the sake of my sanity
/*
12/11/24 Note: This is probably what makes compiling take so fucking long, since every god damned header and source file includes this... oh wait...
no, I don't think that this should cause compiling to take longer since the #ifndef means that these headers only actually get copied by the preprocessor
once... I think, at least. This could just be a misnomer/not how that works(?) I need to make sure.
*/
#ifndef GRAPHX_SANITY
#define GRAPHX_SANITY
#include <iostream>

#include <images.h>
#include <shaders.hpp>

#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

#define PRINT_MARKER std::cout << std::endl << "[=======================================]" << std::endl;
#define PRINTERR(thing) std::cerr << std::endl << "[ERROR]\t" << thing << std::endl;
#define PRINTIMPORTANT(thing) for(int i = 0 ; i < 10 ; i++) { std::cout << std::endl << "[!]\t" << thing; }; std::cout << std::endl;
#define PRINTNOTE(thing) std::cout << std::endl << "[NOTE]\t" << thing << std::endl;
#ifdef GRAPHX_DEBUG
#define TICK(tick) std::cout << "[TICK #" << tick << "]" << std::endl;
#define PRINT(thing) std::cout << thing;
#define PRINTLN(thing) std::cout << std::endl << thing << std::endl;
#define PRINTDEBUG(thing) std::cout << "[DEBUG]\t" << thing << std::endl;
#define JOLTDEBUG(thing) std::cout << "[JOLT]\t" << thing << std::endl;
#else
#define TICK(tick);
#define PRINT(thing);
#define PRINTLN(thing);
#define PRINTDEBUG(thing); // Might change this to print nothing, but I like having a little marker to remind/notify me that there are debug messages in the code somewhere
#define JOLTDEBUG(thing);
#endif
#endif