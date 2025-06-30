#ifndef ENGINE_H
#define ENGINE_H

#include "../lib/quickjs/quickjs.h"

/// @brief Spawns a quickjs runtime
/// @param rt Return param
/// @param ctx Return param
/// @return 0 indicates if initialization was successful
int engine_init(JSRuntime **rt, JSContext **ctx);

/// @brief Deallocates quickjs runtime
/// @param ctx Context which should be deallocated
void engine_deinit(JSContext *ctx);

/// @brief Checks if an exception was thrown and prints it formatted to stderr.
/// @param ctx Context
/// @return false (0) if no exception occured, true otherwise
int engine_exception_handled(JSContext *ctx);

#endif