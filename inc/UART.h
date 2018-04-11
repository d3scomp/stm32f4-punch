#pragma once

#include <cstdint>

extern "C" {
	int _write(int file, char* ptr, int len);
}

/** Custom implementation of _write function.
 * 
 * This would be syscall, but since we do not have OS we need to implement it
 * ourself by print to UART console.
 */
int _write(int file, char* ptr, int len);

/**
 * Initialize UART2 to be used as console
 **/
void initUARTConsole(const uint32_t baud);
