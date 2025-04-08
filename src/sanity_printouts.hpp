#ifndef GRAPHX_SANITY_PRINTOUTS
#define GRAPHX_SANITY_PRINTOUTS
#include <iostream>
#define PRINT_MARKER std::cout << std::endl << "[=======================================]" << std::endl;
#define PRINTERR(thing) std::cerr << std::boolalpha << std::endl << "[ERROR]    " << thing << std::endl;
#define PRINTNOTE(thing) std::cout << std::boolalpha << std::endl << "[NOTE]    " << thing << std::endl;
#ifdef GRAPHX_DEBUG
#ifdef GRAPHX_WINDOWS
#define TICK(tick) OutputDebugString(Lstd::string("[TICK #" + tick + "]\r\n").c_str());
#define PRINT(thing) OutputDebugString(Lstd::string(thing).c_str());
#define PRINTLN(thing) OutputDebugString(Lstd::string("\r\n" + thing + "\r\n").c_str());
#define PRINTDEBUG(thing) OutputDebugString(Lstd::string("[DEBUG]    " + thing + "\r\n").c_str());
#define JOLTDEBUG(thing) OutputDebugString(Lstd::string("[JOLT]    " + thing + "\r\n").c_str());
#else
#define TICK(tick) std::cout << "[TICK #" << tick << "]" << std::endl;
#define PRINT(thing) std::cout << std::boolalpha << thing;
#define PRINTLN(thing) std::cout << std::boolalpha << std::endl << thing << std::endl;
#define PRINTDEBUG(thing) std::cout << std::boolalpha << "[DEBUG]    " << thing << std::endl;
#define JOLTDEBUG(thing) std::cout << std::boolalpha << "[JOLT]    " << thing << std::endl;
#endif
#else
#define TICK(tick);
#define PRINT(thing);
#define PRINTLN(thing);
#define PRINTDEBUG(thing); // Might change this to print nothing, but I like having a little marker to remind/notify me that there are debug messages in the code somewhere
#define JOLTDEBUG(thing);
#endif
#endif