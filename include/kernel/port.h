#ifndef __PORT_H
#define __PORT_H

#include "types.h"

/*
 * Port I/O functions
 */

// === 8-bit port I/O ===

/*
 * @brief Write a byte to the specified port
 * @param port The port number to write to
 * @param data The byte of data to write
 */
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(data), "Nd"(port)
    );
}

/*
 * @brief Read a byte from the specified port
 * @param port The port number to read from
 * @return The byte of data read
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );

    return ret;
}

// === 16-bit port I/O ===

/*
 * @brief Write a word to the specified port
 * @param port The port number to write to
 * @param data The word of data to write
 */
static inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(data), "Nd"(port)
    );
}

/*
 * @brief Read a word from the specified port
 * @param port The port number to read from
 * @return The word of data read
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;

    __asm__ volatile (
        "inw %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );

    return ret;
}

// === 32-bit port I/O ===

/*
 * @brief Write a double word to the specified port
 * @param port The port number to write to
 * @param data The double word of data to write
 */
static inline void outl(uint16_t port, uint32_t data) {
    __asm__ volatile (
        "outl %0, %1"
        :
        : "a"(data), "Nd"(port)
    );
}

/*
 * @brief Read a double word from the specified port
 * @param port The port number to read from
 * @return The double word of data read
 */
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;

    __asm__ volatile (
        "inl %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );

    return ret;
}

// === Implementation of io_wait for slow ports ===

/*
 * @brief Wait for an I/O operation to complete
 * This function is used to introduce a small delay after an I/O operation,
 * which can be necessary for certain hardware devices that require time to
 * process the data.
 */
static inline void io_wait() {
    outb(0x80, 0);
}

#endif // __PORT_H