//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_cdc.c
//
//  Purpose:
//      cdc application  
//
//  Author:
//      
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "app_cdc.h"
#include "drv_bsp.h"
#include "tusb.h"
 
typedef enum {
    LINK_UNMOUNTED = 0,
    LINK_MOUNTED,
    LINK_SUSPENDED,
} LINK_STATE_T;

#define LINE_MAX 128

typedef struct
{
    volatile LINK_STATE_T link;
    uint32_t rx_bytes;
    uint32_t tx_bytes;
    uint32_t lines;
    bool     heartbeat;
    char     line[LINE_MAX];   /* command line buffer (C string: strcmp/strncmp) */
    uint16_t line_len;
} CDC_STATE_T;


static CDC_STATE_T g_cdc = {0};   /* explicit = {0}, no implicit zero-init */

/* ------------------------------------------------------------------------ */
/* Tasks                                                                     */
/* ------------------------------------------------------------------------ */
GlobalType_t app_cdc_init(void)
{
    return RT_OK;
}

static void cdc_write_all(const void* buf, uint32_t len)
{
    const uint8_t* p = (const uint8_t*) buf;
    uint32_t sent = 0;
    const uint32_t deadline = board_millis() + 100;

    while (sent < len)
    {
        if (!tud_cdc_connected()) return;

        uint32_t n = tud_cdc_write(p + sent, len - sent);
        sent += n;
        tud_cdc_write_flush();

        if (sent < len)
        {
            tud_task();
            if ((int32_t)(board_millis() - deadline) >= 0) break;  /* give up */
        }
    }
    g_cdc.tx_bytes += sent;
}

static void cdc_puts(const char* s)
{
    cdc_write_all(s, (uint32_t) strlen(s));
}

static void cdc_printf(const char* fmt, ...)
{
    char buf[192];
    va_list ap;
    va_start(ap, fmt);
    int32_t n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0)
    {
        cdc_write_all(buf, (uint32_t) ((n < (int32_t) sizeof(buf)) ? n : (int32_t) sizeof(buf) - 1));
    }
}

/* Route printf()/puts() to the USB serial port as well (see syscalls.c). */
int _write(int fd, char* ptr, int len)
{
    (void) fd;
    if (len > 0) cdc_write_all(ptr, (uint32_t) len);
    return len;
}

/* ------------------------------------------------------------------------ */
/* Commands                                                                  */
/* ------------------------------------------------------------------------ */
static void print_banner(void)
{
    cdc_puts("\r\n");
    cdc_puts("=============================================\r\n");
    cdc_puts(" STM32H743ZIT6 + TinyUSB  USB CDC test\r\n");
    cdc_printf(" SYSCLK %lu MHz USB %s\r\n",
    (unsigned long) (SystemCoreClock / 1000000U),
    (BOARD_TUD_RHPORT == 0) ? "OTG_FS PA11/PA12" : "OTG_HS PB14/PB15");
    cdc_puts(" Type 'help' for commands. Everything you\r\n"
    " type is echoed back as it arrives.\r\n");
    cdc_puts("=============================================\r\n> ");
}

static void cmd_help(void)
{
    cdc_puts("\r\n"
    "commands:\r\n"
    "  help          this text\r\n"
    "  stats         byte counters and uptime\r\n"
    "  clk           clock tree summary\r\n"
    "  hb            toggle the 1 Hz heartbeat line\r\n"
    "  led on|off    drive the user LED (PG7)\r\n"
    "  flood <n>     send n lines back to back (throughput test)\r\n"
    "  sd            SD card / filesystem status\r\n"
    "  ls            list files on the SD card\r\n"
    "  cat <file>    print a file from the SD card\r\n"
    "  remount       re-read the SD card filesystem (after host edits)\r\n");
}

static void cmd_stats(void)
{
    uint32_t up = board_millis() / 1000U;
    cdc_printf("\r\nuptime %lus  rx %lu bytes  tx %lu bytes  lines %lu  heartbeat %s\r\n",
    (unsigned long) up,
    (unsigned long) g_cdc.rx_bytes,
    (unsigned long) g_cdc.tx_bytes,
    (unsigned long) g_cdc.lines,
    g_cdc.heartbeat ? "on" : "off");
}

static void cmd_clk(void)
{
    cdc_printf("\r\nSYSCLK   %lu Hz\r\n"
    "PLL src  HSE 25 MHz crystal (passive)\r\n"
    "USB clk  HSI48 + CRS auto-trim from host SOF\r\n",
    (unsigned long) SystemCoreClock);
}

static void cmd_flood(const char* arg)
{
    uint32_t n = (uint32_t) strtoul(arg, NULL, 10);
    if (n == 0)     n = 10;
    if (n > 10000)  n = 10000;

    uint32_t t0 = board_millis();
    for (uint32_t i = 1; i <= n; i++)
    {
        cdc_printf("line %lu/%lu  the quick brown fox jumps over the lazy dog\r\n",
        (unsigned long) i, (unsigned long) n);
        tud_task();
    }
    uint32_t dt = board_millis() - t0;
    cdc_printf("sent %lu lines in %lu ms\r\n", (unsigned long) n, (unsigned long) dt);
}

static void handle_line(char* line)
{
    g_cdc.lines++;

    /* trim leading spaces */
    while (*line == ' ') line++;

    if (*line == '\0')
    {
        /* bare Enter - just reprint the prompt */
    }
    else if (strcmp(line, "help") == 0)
    {
        cmd_help();
    }
    else if (strcmp(line, "stats") == 0)
    {
        cmd_stats();
    }
    else if (strcmp(line, "clk") == 0)
    {
        cmd_clk();
    }
    else
    {
        cdc_printf("\r\nunknown command: '%s'  (try 'help')\r\n", line);
    }

    cdc_puts("> ");
}

void app_cdc_task(void)
{
    if (!tud_cdc_available()) return;

    uint8_t buf[64];
    uint32_t count = tud_cdc_read(buf, sizeof(buf));
    g_cdc.rx_bytes += count;

    for (uint32_t i = 0; i < count; i++)
    {
        uint8_t c = buf[i];

        if (c == '\r' || c == '\n')
        {
            cdc_puts("\r\n");
            g_cdc.line[g_cdc.line_len] = '\0';
            g_cdc.line_len = 0;
            handle_line(g_cdc.line);
        }
        else if (c == '\b' || c == 0x7F)
        {
            /* backspace / delete */
            if (g_cdc.line_len > 0)
            {
                g_cdc.line_len--;
                cdc_puts("\b \b");
            }
        }
        else
        {
            if (g_cdc.line_len < LINE_MAX - 1)
            {
                g_cdc.line[g_cdc.line_len++] = c;
            }
            cdc_write_all(&c, 1);                       /* echo as it arrives */
        }
    }
}

/* ------------------------------------------------------------------------ */
/* TinyUSB device callbacks                                                  */
/* ------------------------------------------------------------------------ */
void tud_mount_cb(void)
{
    g_cdc.link = LINK_MOUNTED;
}
void tud_umount_cb(void)
{
    g_cdc.link = LINK_UNMOUNTED;
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    g_cdc.link = LINK_SUSPENDED;
}

void tud_resume_cb(void)
{
    g_cdc.link = LINK_MOUNTED;
}

/* Fired when the host opens or closes the port (DTR assert/deassert). */
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void) itf;
    (void) rts;
    if (dtr)
    {
        /* Small settle delay: some terminals drop the first bytes right after
        * they assert DTR. */
        board_delay_ms(10);
        g_cdc.line_len = 0;
        print_banner();
    }
}
