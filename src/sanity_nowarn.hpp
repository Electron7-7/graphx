#ifndef SANITY_NOWARN
#define SANITY_NOWARN
// https://stackoverflow.com/questions/48426484/concise-way-to-disable-specific-warning-instances-in-clang
#define DO_PRAGMA(x) _Pragma(#x)
#define NOWARN(warnoption, ...)                    \
    DO_PRAGMA(GCC diagnostic push)                 \
    DO_PRAGMA(GCC diagnostic ignored #warnoption)  \
    __VA_ARGS__                                    \
    DO_PRAGMA(GCC diagnostic pop)
#endif