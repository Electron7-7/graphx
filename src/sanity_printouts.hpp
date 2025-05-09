#ifndef GRAPHX_SANITY_PRINTOUTS
#define GRAPHX_SANITY_PRINTOUTS
#include <iostream>
#ifdef GRAPHX_WINDOWS
#define TICK(tick) OutputDebugString(Lstd::string("[TICK #" + tick + "]\r\n").c_str());
#define PRINT(thing) OutputDebugString(Lstd::string(thing).c_str());
#define PRINTDEBUG(thing) OutputDebugString(Lstd::string("[DEBUG]    " + thing + "\r\n").c_str());
#define JOLTDEBUG(thing) OutputDebugString(Lstd::string("[JOLT]    " + thing + "\r\n").c_str());
#define PRINTERR(thing) OutputDebugString(Lstd::string("[ERROR]    " + thing + "\r\n").c_str());
#define PRINTWARN(thing) OutputDebugString(Lstd::string("[WARNING]    " + thing + "\r\n").c_str());
#define PRINTNOTE(thing) OutputDebugString(Lstd::string("[NOTE]    " + thing + "\r\n").c_str());
#else
#define TICK(tick) std::cout << "[TICK #" << std::to_string(tick) << "]" << std::endl;
#define PRINT(thing) std::cout << std::boolalpha << thing << std::endl;
#define PRINTDEBUG(thing) std::cout << std::boolalpha << "[DEBUG]    " << thing << std::endl;
#define JOLTDEBUG(thing) std::cout << std::boolalpha << "[JOLT]    " << thing << std::endl;
#define PRINTERR(thing) std::cerr << std::boolalpha << '\n' << "[ERROR]    " << thing << std::endl;
#define PRINTWARN(thing) std::cout << std::boolalpha << "[WARNING]    " << thing << std::endl;
#define PRINTNOTE(thing) std::cout << std::boolalpha << '\n' << "[NOTE]    " << thing << std::endl;
#endif
#ifndef GRAPHX_DEBUG
#define TICK(tick);
#define PRINT(thing);
#define PRINTDEBUG(thing);
#define JOLTDEBUG(thing);
#endif
#endif