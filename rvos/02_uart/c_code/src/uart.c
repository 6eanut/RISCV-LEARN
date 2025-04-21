#include "../include/platform.h"
#include "../include/types.h"

/*
 * UART control registers map. see [1] "PROGRAMMING TABLE"
 * note some are reused by multiple functions
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 */
#define RHR 0 // Receive Holding Register (read mode)
#define THR 0 // Transmit Holding Register (write mode)
#define DLL 0 // LSB of Divisor Latch (write mode)
#define IER 1 // Interrupt Enable Register (write mode)
#define DLM 1 // MSB of Divisor Latch (write mode)
#define FCR 2 // FIFO Control Register (write mode)
#define ISR 2 // Interrupt Status Register (read mode)
#define LCR 3 // Line Control Register
#define MCR 4 // Modem Control Register
#define LSR 5 // Line Status Register
#define MSR 6 // Modem Status Register
#define SPR 7 // ScratchPad Register

// every reg has 8 bits
#define UART_REG(reg) ((volatile uint8_t *)UART0 + reg)

// lunxun read or write ready
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE (1 << 5)

// write or read reg
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))
#define uart_read_reg(reg) (*(UART_REG(reg)))

void uart_init()
{
    // disable interrupt
    uart_write_reg(IER, 0x00);

    // set baud rate, composed of DLL and DLM (16 bits)
    // LCR decides 0 reprsents which reg, 1 represents which reg
    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 0x03);
    uart_write_reg(DLM, 0x00);

    // number of the data bits
    lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));
}

void uart_putc(char ch)
{
    while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0)
        ;
    uart_write_reg(THR, ch);
}

void uart_puts(char *s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}