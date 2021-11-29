#ifndef DRIVER_IO_H
#define DRIVER_IO_H
#include "../driver/io.h"
#endif

#ifndef DRIVER_SYS_H
#define DRIVER_SYS_H
#include "../driver/sys.h"
#endif

void panic() {
    puts("panic\n");
    exit(1);
}