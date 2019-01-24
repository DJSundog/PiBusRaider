#include "postman.h"
#include "ee_printf.h"
#include "lowlev.h"
#include "lowlib.h"

//#define POSTMAN_DEBUG 1

#define MAPPED_REGISTERS_BASE 0x20000000

static volatile unsigned int* MAILBOX0READ = (unsigned int*)lowlev_mem_p2v(MAPPED_REGISTERS_BASE + 0xB880);
static volatile unsigned int* MAILBOX0STATUS = (unsigned int*)lowlev_mem_p2v(MAPPED_REGISTERS_BASE + 0xB898);
static volatile unsigned int* MAILBOX0WRITE = (unsigned int*)lowlev_mem_p2v(MAPPED_REGISTERS_BASE + 0xB8A0);

POSTMAN_RETURN_TYPE postman_recv(unsigned int channel, unsigned int* out_data)
{
#ifdef POSTMAN_DEBUG
    uart_printf("Postman recv from channel 0x%x\n", channel);
#endif

    if (channel > 0xF) {
        return POSTMAN_BAD_DATA;
    }

    unsigned int n_skipped = 0;
    unsigned int start_time = micros();

    while (n_skipped < MAILBOX_MAX_MSG_TO_SKIP) {
        // waits for mailbox being ready
        lowlev_flushcache();
        while (*MAILBOX0STATUS & 0x40000000) //30th bit is zero when ready
        {
#ifdef POSTMAN_DEBUG
            uart_printf("Mailbox empty, waiting...\n");
#endif
            if (isTimeout(micros(), start_time, MAILBOX_WAIT_TIMEOUT)) {
                return POSTMAN_RECV_TIMEOUT;
            }
            lowlev_flushcache();
        }

        // read the message
        lowlev_dmb();
        unsigned int msg = *MAILBOX0READ;
        lowlev_dmb();

#ifdef POSTMAN_DEBUG
        uart_printf("Received from channel 0x%x\n", msg & 0xf);
#endif

        // check mailbox id
        if ((msg & 0xF) == (channel & 0xF)) {
            // mailbox channel ok, return the data
            *out_data = msg >> 4;
            return POSTMAN_SUCCESS;
        }

        if (isTimeout(micros(), start_time, MAILBOX_WAIT_TIMEOUT)) {
            return POSTMAN_RECV_TIMEOUT;
        }

        ++n_skipped;
    }

    return POSTMAN_TOO_MANY_MSG;
}

POSTMAN_RETURN_TYPE postman_send(unsigned int channel, unsigned int data)
{
#ifdef POSTMAN_DEBUG
    char debug_buff[20] = { 0 };
    uart_printf("Postman send to channel 0x%s\n", channel);
#endif

    if (data & 0xF) {
        // lowest 4-bits of data should be zero, aborting
        return POSTMAN_BAD_DATA;
    }

    // waits for mailbox being ready
    unsigned int start_time = micros();
    while (*MAILBOX0STATUS & 0x80000000) //top bit is zero when ready
    {
#ifdef POSTMAN_DEBUG
        uart_printf("Mailbox full, waiting...\n");
#endif
        if (isTimeout(micros(), start_time, MAILBOX_WAIT_TIMEOUT)) {
            return POSTMAN_SEND_TIMEOUT;
        }
    }

    lowlev_dmb();
    *MAILBOX0WRITE = data | channel; //lowest 4 bits for the mailbox, top 28 bits for the data

#ifdef POSTMAN_DEBUG
    uart_printf("Message sent.\n");
#endif

    return POSTMAN_SUCCESS;
}
