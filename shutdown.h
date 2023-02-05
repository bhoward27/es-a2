#ifndef SHUTDOWN_H_
#define SHUTDOWN_H_

#include <stdatomic.h>
#include <stdbool.h>

bool isShutdownRequested(void);
void requestShutdown(void);

#endif
