/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how USB works as a dual role device.
 *           If it works as USB Host, it can access a mass storage device.
 *           If it works as USB Device, it acts as a mass storage device.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "usbh_lib.h"
#include "ff.h"
#include "diskio.h"
#include "massstorage.h"

#define USE_USB_APLL1_CLOCK         1
static uint8_t s_u8IsAdevice = 0;

void SysTick_Handler(void);
void enable_sys_tick(int ticks_per_second);
void delay_us(int usec);
void dump_buff_hex(uint8_t *pu8Buff, int i8Bytes);
int xatoi(TCHAR **str, long *res);
void put_dump(const unsigned char *buff, unsigned long addr, int cnt);
static FRESULT scan_files(char *path);
void put_rc(FRESULT rc);
void get_line(char *buff, int len);
DWORD get_fattime(void);
void SYS_Init(void);
void USBH_Process(void);

/* OTG interrupt handler */
NVT_ITCM void USBOTG_IRQHandler(void)
{
    __IO uint32_t u32INTSTS, u32INTEN;

    u32INTEN = OTG->INTEN;
    u32INTSTS = OTG->INTSTS;

    if (u32INTSTS & u32INTEN & OTG_INTSTS_IDCHGIF_Msk)
    {
        printf("[ID changed]\n");

        /* Check ID status */
        if (OTG_GET_STATUS(OTG_STATUS_IDSTS_Msk))
            s_u8IsAdevice = 0;

        /* Clear ID status changed interrupt flag */
        OTG_CLR_INT_FLAG(OTG_INTSTS_IDCHGIF_Msk);
    }

    if (u32INTSTS & u32INTEN & OTG_INTSTS_BVLDCHGIF_Msk)
    {
        printf("[B session valid (OTG_STATUS: 0x%x)]\n", OTG->STATUS);

        /* Check ID status */
        if (OTG_GET_STATUS(OTG_STATUS_IDSTS_Msk) == 0)
            s_u8IsAdevice = 1;

        /* Clear B-device session valid state change interrupt flag */
        OTG_CLR_INT_FLAG(OTG_INTSTS_BVLDCHGIF_Msk);
    }

    /* Clear all interrupt flags */
    OTG->INTSTS = u32INTSTS;
    /* make sure that interrupt flag has been cleared. */
    u32INTSTS = OTG->INTSTS;
}

#define BUFF_SIZE       (2048)

static UINT g_u8Len = BUFF_SIZE;
static DWORD s_u32AccSize;
static WORD s_u16AccFiles, s_u16AccDirs;
static FILINFO Finfo;
static FIL file1, file2;                   /* File objects                               */

static char s_achLine[256];                /* Console input buffer                       */
#if _USE_LFN
    char g_achLfname[512];                     /* Console input buffer                       */
#endif

#ifdef __ICCARM__
    #pragma data_alignment=32
    BYTE s_au8BuffPool1[BUFF_SIZE];         /* Working buffer 1                           */
    BYTE s_au8BuffPool2[BUFF_SIZE];         /* Working buffer 2                           */
#else
    static BYTE s_au8BuffPool1[BUFF_SIZE] __attribute__((aligned(32)));    /* Working buffer 1                           */
    static BYTE s_au8BuffPool2[BUFF_SIZE] __attribute__((aligned(32)));    /* Working buffer 2                           */
#endif

static BYTE *s_pu8Buff1;
static BYTE *s_pu8Buff2;

static volatile uint32_t s_u32TickCnt;

NVT_ITCM void SysTick_Handler(void)
{
    s_u32TickCnt++;
}

void enable_sys_tick(int ticks_per_second)
{
    s_u32TickCnt = 0;

    if (SysTick_Config(SystemCoreClock / (uint32_t)ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");

        while (1);
    }
}

uint32_t get_ticks(void)
{
    return s_u32TickCnt;
}

/*
 *  This function is necessary for USB Host library.
 */
void delay_us(int usec)
{
    /*
     *  Configure Timer0, clock source from HIRC_12M. Prescale 12
     */
    /* TIMER0 clock from HIRC */
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
    CLK_EnableModuleClock(TMR0_MODULE);

    TIMER_SET_PRESCALE_VALUE(TIMER0, (12 - 1));
    /* stop timer0 */
    TIMER_Stop(TIMER0);
    /* write 1 to clear for safety */
    TIMER_ClearIntFlag(TIMER0);
    TIMER_ClearWakeupFlag(TIMER0);
    /* set timer cmp value */
    TIMER_SET_CMP_VALUE(TIMER0, usec);
    /* Timer0 config to oneshot mode */
    TIMER_SET_OPMODE(TIMER0, TIMER_ONESHOT_MODE);
    /* start timer0*/
    TIMER_Start(TIMER0);

    while (TIMER_GetIntFlag(TIMER0) == 0);
}

void dump_buff_hex(uint8_t *pu8Buff, int i8Bytes)
{
    int i8Idx, i8Cnt;

    i8Idx = 0;

    while (i8Bytes > 0)
    {
        printf("0x%04X  ", i8Idx);

        for (i8Cnt = 0; i8Cnt < 16; i8Cnt++)
            printf("%02x ", pu8Buff[i8Idx + i8Cnt]);

        printf("  ");

        for (i8Cnt = 0; i8Cnt < 16; i8Cnt++)
        {
            if ((pu8Buff[i8Idx + i8Cnt] >= 0x20) && (pu8Buff[i8Idx + i8Cnt] < 127))
                printf("%c", pu8Buff[i8Idx + i8Cnt]);
            else
                printf(".");

            i8Bytes--;
        }

        i8Idx += 16;
        printf("\n");
    }

    printf("\n");
}

/*--------------------------------------------------------------------------*/
/* Monitor                                                                  */
/*--------------------------------------------------------------------------*/
/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*  "123 -5   0x3ff 0b1111 0377  w "
        ^                           1st call returns 123 and next ptr
           ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/

int xatoi           /* 0:Failed, 1:Successful */
(
    TCHAR **str,    /* Pointer to pointer to the string */
    long *res       /* Pointer to a variable to store the value */
)
{
    long val;
    unsigned char r, s = 0;
    TCHAR c;

    *res = 0;

    while ((c = **str) == ' ')(*str)++;     /* Skip leading spaces */

    if (c == '-')       /* negative? */
    {
        s = 1;
        c = *(++(*str));
    }

    if (c == '0')
    {
        c = *(++(*str));

        switch (c)
        {
            case 'x':       /* hexadecimal */
                r = 16;
                c = *(++(*str));
                break;

            case 'b':       /* binary */
                r = 2;
                c = *(++(*str));
                break;

            default:
                if (c <= ' ') return 1; /* single zero */

                if (c < '0' || c > '9') return 0;   /* invalid char */

                r = 8;      /* octal */
        }
    }
    else
    {
        if (c < '0' || c > '9') return 0;   /* EOL or invalid char */

        r = 10;         /* decimal */
    }

    val = 0;

    while (c > ' ')
    {
        if (c >= 'a') c -= 0x20;

        c -= '0';

        if (c >= 17)
        {
            c -= 7;

            if (c <= 9) return 0;   /* invalid char */
        }

        if (c >= r) return 0;       /* invalid char for current radix */

        val = val * r + c;
        c = *(++(*str));
    }

    if (s) val = 0 - val;           /* apply sign if needed */

    *res = val;
    return 1;
}

/*----------------------------------------------*/
/* Dump a block of byte array                   */
/*----------------------------------------------*/
void put_dump
(
    const unsigned char *buff,  /* Pointer to the byte array to be dumped */
    unsigned long addr,         /* Heading address value */
    int cnt                     /* Number of bytes to be dumped */
)
{
    int i;


    printf("%08x ", (UINT)addr);

    for (i = 0; i < cnt; i++)
        printf(" %02x", buff[i]);

    printf(" ");

    for (i = 0; i < cnt; i++)
        putchar((TCHAR)((buff[i] >= ' ' && buff[i] <= '~') ? buff[i] : '.'));

    printf("\n");
}

/*--------------------------------------------------------------------------*/
/* Monitor                                                                  */
/*--------------------------------------------------------------------------*/
static FRESULT scan_files(char *path)
{
    DIR dirs;
    FRESULT res;
    uint32_t i;
    char *fn;

    if ((res = f_opendir(&dirs, path)) == FR_OK)
    {
        i = strlen(path);

        while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0])
        {
            if (FF_FS_RPATH && Finfo.fname[0] == '.') continue;

#if _USE_LFN
            fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
            fn = Finfo.fname;
#endif

            if (Finfo.fattrib & AM_DIR)
            {
                s_u16AccDirs++;
                *(path + i) = '/';
                strcpy(path + i + 1, fn);
                res = scan_files(path);
                *(path + i) = '\0';

                if (res != FR_OK) break;
            }
            else
            {
                /*              printf("%s/%s\n", path, fn); */
                s_u16AccFiles++;
                s_u32AccSize += Finfo.fsize;
            }
        }
    }

    return res;
}

void put_rc(FRESULT rc)
{
    const TCHAR *p =
        _T("OK\0DISK_ERR\0INT_ERR\0NOT_READY\0NO_FILE\0NO_PATH\0INVALID_NAME\0")
        _T("DENIED\0EXIST\0INVALID_OBJECT\0WRITE_PROTECTED\0INVALID_DRIVE\0")
        _T("NOT_ENABLED\0NO_FILE_SYSTEM\0MKFS_ABORTED\0TIMEOUT\0LOCKED\0")
        _T("NOT_ENOUGH_CORE\0TOO_MANY_OPEN_FILES\0");
    //FRESULT i;
    uint32_t i;

    for (i = 0; (i != (UINT)rc) && *p; i++)
    {
        while (*p++) ;
    }

    printf(_T("rc=%d FR_%s\n"), (UINT)rc, p);
}

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/
void get_line(char *buff, int len)
{
    int32_t c;
    int idx = 0;

    for (;;)
    {
        c = getchar();
        putchar(c);

        if (c == '\r') break;

        if ((c == '\b') && idx) idx--;

        if ((c >= ' ') && (idx < len - 1)) buff[idx++] = (char)c;
    }

    buff[idx] = 0;
    putchar('\n');
}

/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */
/*---------------------------------------------------------*/
DWORD get_fattime(void)
{
    DWORD tmr;

    tmr = 0x00000;

    return tmr;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);

    /* Wait for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

    /* Switch SCLK clock source to PLL0 and Enable PLL0 220MHz clock */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_APLLCTL_APLLSRC_HXT, FREQ_220MHZ);

#if (USE_USB_APLL1_CLOCK)
    /* Enable APLL1 96MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HXT, 96000000, CLK_APLL1_SELECT);
#endif

    /* Enable GPIOA module clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

#if (USE_USB_APLL1_CLOCK)
    /* USB Host desired input clock is 48 MHz. Set as APLL1 divided by 2 (96/2 = 48) */
    CLK_SetModuleClock(USBH0_MODULE, CLK_USBSEL_USBSEL_APLL1_DIV2, CLK_USBDIV_USBDIV(1));
#else
    /* USB Host desired input clock is 48 MHz. Set as HIRC48M divided by 1 (48/1 = 48) */
    CLK_SetModuleClock(USBH0_MODULE, CLK_USBSEL_USBSEL_HIRC48M, CLK_USBDIV_USBDIV(1));
#endif

    /* Enable USBH module clock */
    CLK_EnableModuleClock(USBH0_MODULE);

    /* Enable USBD module clock */
    CLK_EnableModuleClock(USBD0_MODULE);

    /* Enable OTG module clock */
    CLK_EnableModuleClock(OTG0_MODULE);

    /* Enable UART module clock */
    SetDebugUartCLK();

    /* Set OTG as USB Host role */
    SYS->USBPHY = (0x1ul << (SYS_USBPHY_HSOTGPHYEN_Pos)) | (0x2ul << (SYS_USBPHY_HSUSBROLE_Pos)) | (0x1ul << (SYS_USBPHY_OTGPHYEN_Pos)) | (0x2 << SYS_USBPHY_USBROLE_Pos);
    delay_us(20);
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;//Set HSUSB PHY Active.

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART RXD and TXD */
    SetDebugUartMFP();

    /* USB_VBUS_EN (USB 1.1 VBUS power enable pin) multi-function pin - PB.15     */
    SET_USB_VBUS_EN_PB15();

    /* USB_VBUS_ST (USB 1.1 over-current detect pin) multi-function pin - PB.14   */
    SET_USB_VBUS_ST_PB14();

    /* USB 1.1 port multi-function pin VBUS, D+, D-, and ID pins */
    SET_USB_VBUS_PA12();
    SET_USB_D_MINUS_PA13();
    SET_USB_D_PLUS_PA14();
    SET_USB_OTG_ID_PA15();

    SystemCoreClockUpdate();
    /* Lock protected registers */
    SYS_LockReg();

}

void USBH_Process(void)
{
    char *ptr, *ptr2;
    unsigned long p1, p2, p3;
    BYTE *buf;
    FATFS *fs;              /* Pointer to file system object */
    FRESULT res;
    TCHAR usb_path[] = { '3', ':', 0 };    /* USB drive started from 3 */

    DIR dir;                /* Directory object */
    UINT s1, s2, cnt;
    static const BYTE ft[] = {0, 12, 16, 32};
    DWORD ofs = 0, sect = 0;
    uint32_t t0;

    usbh_pooling_hubs();
    f_chdrive(usb_path);          /* set default path */

    for (;;)
    {
        if (!s_u8IsAdevice)
        {
            printf("break-A (OTG_STATUS: 0x%x)\n", OTG->STATUS);
            return;
        }

        usbh_pooling_hubs();

        printf(_T(">"));
        ptr = s_achLine;

        get_line(ptr, sizeof(s_achLine));

        switch (*ptr++)
        {
            case '4' :
            case '5' :
            case '6' :
            case '7' :
                ptr--;
                *(ptr + 1) = ':';
                *(ptr + 2) = 0;
                put_rc(f_chdrive((TCHAR *)ptr));
                break;

            case 'd' :     /* d [<lba>] - Dump sector */
                if (!xatoi(&ptr, (long *)&p2)) p2 = sect;

                res = (FRESULT)disk_read(3, s_pu8Buff1, p2, 1);

                if (res)
                {
                    printf("rc=%d\n", (WORD)res);
                    break;
                }

                sect = p2 + 1;
                printf("Sector:%d\n", (INT)p2);

                for (buf = (unsigned char *)s_pu8Buff1, ofs = 0; ofs < 0x200; buf += 16, ofs += 16)
                    put_dump(buf, ofs, 16);

                break;

            case 'b' :
                switch (*ptr++)
                {
                    case 'd' :  /* bd <addr> - Dump R/W buffer */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        for (ptr = (char *)&s_pu8Buff1[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr += 16, ofs += 16)
                            put_dump((BYTE *)ptr, ofs, 16);

                        break;

                    case 'e' :  /* be <addr> [<data>] ... - Edit R/W buffer */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        if (xatoi(&ptr, (long *)&p2))
                        {
                            do
                            {
                                s_pu8Buff1[p1++] = (BYTE)p2;
                            } while (xatoi(&ptr, (long *)&p2));

                            break;
                        }

                        for (;;)
                        {
                            printf("%04X %02X-", (WORD)p1, s_pu8Buff1[p1]);
                            get_line(s_achLine, sizeof(s_achLine));
                            ptr = s_achLine;

                            if (*ptr == '.') break;

                            if (*ptr < ' ')
                            {
                                p1++;
                                continue;
                            }

                            if (xatoi(&ptr, (long *)&p2))
                                s_pu8Buff1[p1++] = (BYTE)p2;
                            else
                                printf("???\n");
                        }

                        break;

                    case 'r' :  /* br <sector> [<n>] - Read disk into R/W buffer */
                        if (!xatoi(&ptr, (long *)&p2)) break;

                        if (!xatoi(&ptr, (long *)&p3)) p3 = 1;

                        printf("rc=%d\n", disk_read(0, s_pu8Buff1, p2, p3));
                        break;

                    case 'w' :  /* bw <sector> [<n>] - Write R/W buffer into disk */
                        if (!xatoi(&ptr, (long *)&p2)) break;

                        if (!xatoi(&ptr, (long *)&p3)) p3 = 1;

                        printf("rc=%d\n", disk_write(0, s_pu8Buff1, p2, p3));
                        break;

                    case 'f' :  /* bf <n> - Fill working buffer */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        memset(s_pu8Buff1, (int)p1, BUFF_SIZE);
                        break;

                }

                break;

            case 'f' :
                switch (*ptr++)
                {

                    case 's' :  /* fs - Show logical drive status */
                        res = f_getfree("", (DWORD *)&p2, &fs);

                        if (res)
                        {
                            put_rc(res);
                            break;
                        }

                        printf("FAT type = FAT%d\nBytes/Cluster = %d\nNumber of FATs = %d\n"
                               "Root DIR entries = %d\nSectors/FAT = %d\nNumber of clusters = %d\n"
                               "FAT start (lba) = %d\nDIR start (lba,cluster) = %d\nData start (lba) = %d\n\n...",
                               ft[fs->fs_type & 3], (UINT)(fs->csize * 512UL), fs->n_fats,
                               fs->n_rootdir, (INT)fs->fsize, (INT)(fs->n_fatent - 2),
                               (INT)fs->fatbase, (INT)fs->dirbase, (INT)fs->database
                              );
                        s_u32AccSize = s_u16AccFiles = s_u16AccDirs = 0;
#if _USE_LFN
                        Finfo.lfname = g_achLfname;
                        Finfo.lfsize = sizeof(g_achLfname);
#endif
                        res = scan_files(ptr);

                        if (res)
                        {
                            put_rc(res);
                            break;
                        }

                        printf("\r%d files, %d bytes.\n%d folders.\n"
                               "%d KB total disk space.\n%d KB available.\n",
                               s_u16AccFiles, (INT)s_u32AccSize, s_u16AccDirs,
                               (INT)((fs->n_fatent - 2) * (fs->csize / 2)), (INT)(p2 * (fs->csize / 2))
                              );
                        break;

                    case 'l' :  /* fl [<path>] - Directory listing */
                        while (*ptr == ' ') ptr++;

                        res = f_opendir(&dir, ptr);

                        if (res)
                        {
                            put_rc(res);
                            break;
                        }

                        p1 = s1 = s2 = 0;

                        for (;;)
                        {
                            res = f_readdir(&dir, &Finfo);

                            if ((res != FR_OK) || !Finfo.fname[0]) break;

                            if (Finfo.fattrib & AM_DIR)
                            {
                                s2++;
                            }
                            else
                            {
                                s1++;
                                p1 += Finfo.fsize;
                            }

                            printf("%c%c%c%c%c %d/%02d/%02d %02d:%02d    %9d  %s",
                                   (Finfo.fattrib & AM_DIR) ? 'D' : '-',
                                   (Finfo.fattrib & AM_RDO) ? 'R' : '-',
                                   (Finfo.fattrib & AM_HID) ? 'H' : '-',
                                   (Finfo.fattrib & AM_SYS) ? 'S' : '-',
                                   (Finfo.fattrib & AM_ARC) ? 'A' : '-',
                                   (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                                   (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, (INT)Finfo.fsize, Finfo.fname);
#if _USE_LFN

                            for (p2 = strlen(Finfo.fname); p2 < 14; p2++)
                                printf(" ");

                            printf("%s\n", g_achLfname);
#else
                            printf("\n");
#endif
                        }

                        printf("%4d File(s),%10d bytes total\n%4d Dir(s)", s1, (INT)p1, s2);

                        if (f_getfree(ptr, (DWORD *)&p1, &fs) == FR_OK)
                            printf(", %10d bytes free\n", (INT)(p1 * fs->csize * 512));

                        break;


                    case 'o' :  /* fo <mode> <file> - Open a file */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        while (*ptr == ' ') ptr++;

                        put_rc(f_open(&file1, ptr, (BYTE)p1));
                        break;

                    case 'c' :  /* fc - Close a file */
                        put_rc(f_close(&file1));
                        break;

                    case 'e' :  /* fe - Seek file pointer */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        res = f_lseek(&file1, p1);
                        put_rc(res);

                        if (res == FR_OK)
                            printf("fptr=%d(0x%X)\n", (INT)file1.fptr, file1.fptr);

                        break;

                    case 'd' :  /* fd <len> - read and dump file from current fp */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        ofs = file1.fptr;

                        while (p1)
                        {
                            if ((UINT)p1 >= 16)
                            {
                                cnt = 16;
                                p1 -= 16;
                            }
                            else
                            {
                                cnt = p1;
                                p1 = 0;
                            }

                            res = f_read(&file1, s_pu8Buff1, cnt, &cnt);

                            if (res != FR_OK)
                            {
                                put_rc(res);
                                break;
                            }

                            if (!cnt) break;

                            put_dump(s_pu8Buff1, ofs, (int)cnt);
                            ofs += 16;
                        }

                        break;

                    case 'r' :  /* fr <len> - read file */
                        if (!xatoi(&ptr, (long *)&p1)) break;

                        p2 = 0;
                        t0 = get_ticks();

                        while (p1)
                        {
                            if ((UINT)p1 >= g_u8Len)
                            {
                                cnt = g_u8Len;
                                p1 -= g_u8Len;
                            }
                            else
                            {
                                cnt = p1;
                                p1 = 0;
                            }

                            res = f_read(&file1, s_pu8Buff1, cnt, &s2);

                            if (res != FR_OK)
                            {
                                put_rc(res);
                                break;
                            }

                            p2 += s2;

                            if (cnt != s2) break;
                        }

                        p1 = get_ticks() - t0;

                        if (p1)
                            printf("%d bytes read with %d kB/sec.\n", (INT)p2, (INT)(((p2 * 100) / p1) / 1024));

                        break;

                    case 'w' :  /* fw <len> <val> - write file */
                        if (!xatoi(&ptr, (long *)&p1) || !xatoi(&ptr, (long *)&p2)) break;

                        memset(s_pu8Buff1, (BYTE)p2, g_u8Len);
                        p2 = 0;
                        t0 = get_ticks();

                        while (p1)
                        {
                            if ((UINT)p1 >= g_u8Len)
                            {
                                cnt = g_u8Len;
                                p1 -= g_u8Len;
                            }
                            else
                            {
                                cnt = p1;
                                p1 = 0;
                            }

                            res = f_write(&file1, s_pu8Buff1, cnt, &s2);

                            if (res != FR_OK)
                            {
                                put_rc(res);
                                break;
                            }

                            p2 += s2;

                            if (cnt != s2) break;
                        }

                        p1 = get_ticks() - t0;

                        if (p1)
                            printf("%d bytes written with %d kB/sec.\n", (INT)p2, (INT)(((p2 * 100) / p1) / 1024));

                        break;

                    case 'n' :  /* fn <old_name> <new_name> - Change file/dir name */
                        while (*ptr == ' ') ptr++;

                        ptr2 = strchr(ptr, ' ');

                        if (!ptr2) break;

                        *ptr2++ = 0;

                        while (*ptr2 == ' ') ptr2++;

                        put_rc(f_rename(ptr, ptr2));
                        break;

                    case 'u' :  /* fu <name> - Unlink a file or dir */
                        while (*ptr == ' ') ptr++;

                        put_rc(f_unlink(ptr));
                        break;

                    case 'v' :  /* fv - Truncate file */
                        put_rc(f_truncate(&file1));
                        break;

                    case 'k' :  /* fk <name> - Create a directory */
                        while (*ptr == ' ') ptr++;

                        put_rc(f_mkdir(ptr));
                        break;

                    case 'a' :  /* fa <atrr> <mask> <name> - Change file/dir attribute */
                        if (!xatoi(&ptr, (long *)&p1) || !xatoi(&ptr, (long *)&p2)) break;

                        while (*ptr == ' ') ptr++;

                        put_rc(f_chmod(ptr, (BYTE)p1, (BYTE)p2));
                        break;

                    case 't' :  /* ft <year> <month> <day> <hour> <min> <sec> <name> - Change time stamp */
                        if (!xatoi(&ptr, (long *)&p1) || !xatoi(&ptr, (long *)&p2) || !xatoi(&ptr, (long *)&p3)) break;

                        Finfo.fdate = (WORD)(((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31));

                        if (!xatoi(&ptr, (long *)&p1) || !xatoi(&ptr, (long *)&p2) || !xatoi(&ptr, (long *)&p3)) break;

                        Finfo.ftime = (WORD)(((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31));
                        put_rc(f_utime(ptr, &Finfo));
                        break;

                    case 'x' : /* fx <src_name> <dst_name> - Copy file */
                        while (*ptr == ' ') ptr++;

                        ptr2 = strchr(ptr, ' ');

                        if (!ptr2) break;

                        *ptr2++ = 0;

                        while (*ptr2 == ' ') ptr2++;

                        printf("Opening \"%s\"", ptr);
                        res = f_open(&file1, ptr, FA_OPEN_EXISTING | FA_READ);
                        printf("\n");

                        if (res)
                        {
                            put_rc(res);
                            break;
                        }

                        printf("Creating \"%s\"", ptr2);
                        res = f_open(&file2, ptr2, FA_CREATE_ALWAYS | FA_WRITE);
                        putchar('\n');

                        if (res)
                        {
                            put_rc(res);
                            f_close(&file1);
                            break;
                        }

                        printf("Copying...");
                        p1 = 0;

                        for (;;)
                        {
                            res = f_read(&file1, s_pu8Buff1, BUFF_SIZE, &s1);

                            if (res || s1 == 0) break;   /* error or eof */

                            res = f_write(&file2, s_pu8Buff1, s1, &s2);
                            p1 += s2;

                            if (res || s2 < s1) break;   /* error or disk full */
                        }

                        printf("\n%d bytes copied.\n", (INT)p1);
                        f_close(&file1);
                        f_close(&file2);
                        break;

                    case 'y' : /* fz <src_name> <dst_name> - Compare file */
                        while (*ptr == ' ') ptr++;

                        ptr2 = strchr(ptr, ' ');

                        if (!ptr2) break;

                        *ptr2++ = 0;

                        while (*ptr2 == ' ') ptr2++;

                        printf("Opening \"%s\"", ptr);
                        res = f_open(&file1, ptr, FA_OPEN_EXISTING | FA_READ);
                        printf("\n");

                        if (res)
                        {
                            put_rc(res);
                            break;
                        }

                        printf("Opening \"%s\"", ptr2);
                        res = f_open(&file2, ptr2, FA_OPEN_EXISTING | FA_READ);
                        putchar('\n');

                        if (res)
                        {
                            put_rc(res);
                            f_close(&file1);
                            break;
                        }

                        printf("Compare...");
                        p1 = 0;

                        for (;;)
                        {
                            res = f_read(&file1, s_pu8Buff1, BUFF_SIZE, &s1);

                            if (res || s1 == 0)
                            {
                                printf("\nRead file %s terminated. (%d)\n", ptr, res);
                                break;     /* error or eof */
                            }

                            res = f_read(&file2, s_pu8Buff2, BUFF_SIZE, &s2);

                            if (res || s2 == 0)
                            {
                                printf("\nRead file %s terminated. (%d)\n", ptr2, res);
                                break;     /* error or eof */
                            }

                            p1 += s2;

                            if (res || s2 < s1) break;   /* error or disk full */

                            if (memcmp(s_pu8Buff1, s_pu8Buff2, s1) != 0)
                            {
                                printf("Compare failed!!\n");
                                break;
                            }

                            if ((p1 % 0x10000) == 0)
                                printf("\n%d KB compared.", (INT)(p1 / 1024));

                            printf(".");
                        }

                        if (s1 == 0)
                            printf("\nPASS. \n ");

                        f_close(&file1);
                        f_close(&file2);
                        break;

#if _FS_RPATH

                    case 'g' :  /* fg <path> - Change current directory */
                        while (*ptr == ' ') ptr++;

                        put_rc(f_chdir(ptr));
                        break;

                    case 'j' :  /* fj <drive#> - Change current drive */
                        while (*ptr == ' ') ptr++;

                        dump_buff_hex((uint8_t *)&p1, 16);
                        put_rc(f_chdrive((TCHAR *)ptr));
                        break;
#endif
#if _USE_MKFS

                    case 'm' :  /* fm <partition rule> <sect/clust> - Create file system */
                        if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;

                        printf("The memory card will be formatted. Are you sure? (Y/n)=");
                        get_line(ptr, sizeof(s_achLine));

                        if (*ptr == 'Y')
                            put_rc(f_mkfs(0, (BYTE)p2, (WORD)p3));

                        break;
#endif

                    case 'z' :  /* fz [<rw size>] - Change R/W length for fr/fw/fx command */
                        if (xatoi(&ptr, (long *)&p1) && p1 >= 1 && (size_t)p1 <= BUFF_SIZE)
                            g_u8Len = p1;

                        printf("g_u8Len=%d\n", g_u8Len);
                        break;
                }

                break;

            case '?':       /* Show usage */
                printf(
                    _T("n: - Change default drive (USB drive is 3~7)\n")
                    _T("d [<lba>] - Dump sector\n")
                    //_T("ds <pd#> - Show disk status\n")
                    _T("\n")
                    _T("bd <ofs> - Dump working buffer\n")
                    _T("be <ofs> [<data>] ... - Edit working buffer\n")
                    _T("br <pd#> <sect> [<num>] - Read disk into working buffer\n")
                    _T("bw <pd#> <sect> [<num>] - Write working buffer into disk\n")
                    _T("bf <val> - Fill working buffer\n")
                    _T("\n")
                    _T("fs - Show volume status\n")
                    _T("fl [<path>] - Show a directory\n")
                    _T("fo <mode> <file> - Open a file\n")
                    _T("fc - Close the file\n")
                    _T("fe <ofs> - Move fp in normal seek\n")
                    //_T("fE <ofs> - Move fp in fast seek or Create link table\n")
                    _T("fd <len> - Read and dump the file\n")
                    _T("fr <len> - Read the file\n")
                    _T("fw <len> <val> - Write to the file\n")
                    _T("fn <object name> <new name> - Rename an object\n")
                    _T("fu <object name> - Unlink an object\n")
                    _T("fv - Truncate the file at current fp\n")
                    _T("fk <dir name> - Create a directory\n")
                    _T("fa <atrr> <mask> <object name> - Change object attribute\n")
                    _T("ft <year> <month> <day> <hour> <min> <sec> <object name> - Change timestamp of an object\n")
                    _T("fx <src file> <dst file> - Copy a file\n")
                    _T("fg <path> - Change current directory\n")
                    _T("fj <ld#> - Change current drive. For example: <fj 4:>\n")
                    _T("fm <ld#> <rule> <cluster size> - Create file system\n")
                    _T("\n")
                );
                break;
        }
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    InitDebugUart();                   /* Init DeubgUART for printf */

    /* Lock protected registers */
    SYS_LockReg();

    enable_sys_tick(100);
    printf("\n\n");
    printf("system clock :%uHz\r\n", SystemCoreClock);
    printf("+------------------------------------------------------+\n");
    printf("|                                                      |\n");
    printf("|     OTG ID dependent Mass Storage sample program     |\n");
    printf("|                                                      |\n");
    printf("+------------------------------------------------------+\n");

    printf("The USB role is determined by the ID pin state of USB cable.\n");
    printf("If acts as a USB host, it can access a mass storage device with a simple file system. ");
    printf("Type '?' on command line to show all supported commands.\n");
    printf("If attempts to switch the USB role as a USB device, press 'Enter' key after unplugging the USB cable to exit the file system. ");
    printf("Then plug a proper USB connector to switch the USB role as a mass storage device. The internal data flash is used as the storage.\n");

    OTG_ENABLE_PHY();
    /* Enable ID detection function */
    OTG_ENABLE_ID_DETECT();
    NVIC_EnableIRQ(USBOTG_IRQn);
    delay_us(1000);

    s_pu8Buff1 = (BYTE *)((uint32_t)&s_au8BuffPool1[0]);
    s_pu8Buff2 = (BYTE *)((uint32_t)&s_au8BuffPool2[0]);

    usbh_core_init();
    usbh_umas_init();
    USBH_Process();

    while (1)
    {
        if (OTG_GET_STATUS(OTG_STATUS_IDSTS_Msk))  /* B-device */
        {
            //PA12 is VBUS Detection PIN , it will check status of plug-in.
            if (OTG_GET_STATUS(OTG_STATUS_ASPERI_Msk) && (PA12 == 1))  /* plug-in */
            {
                USBD_Open(&gsInfo, MSC_ClassRequest, NULL);
                USBD_SetConfigCallback(MSC_SetConfig);
                MSC_Init();
                NVIC_EnableIRQ(USBD_IRQn);
                printf("B-device (OTG_STATUS: 0x%x)\n", OTG->STATUS);

                /* Start transaction */
                while (1)
                {
                    if (USBD_IS_ATTACHED())
                    {
                        USBD_Start();
                        break;
                    }

                    if (OTG_GET_STATUS(OTG_STATUS_BVLD_Msk) == 0)
                        break;
                }

                /* Clear B-device session valid state change interrupt flag */
                OTG_CLR_INT_FLAG(OTG_INTSTS_BVLDCHGIF_Msk);
                /* Enable B-device session valid state change interrupt */
                OTG_ENABLE_INT(OTG_INTEN_BVLDCHGIEN_Msk);

                while (1)
                {
                    if (OTG_GET_STATUS(OTG_STATUS_BVLD_Msk) == 0)
                        break;

                    MSC_ProcessCmd();
                }

                /* Disable B-device session valid state change interrupt */
                OTG->INTEN &= ~OTG_INTEN_BVLDCHGIEN_Msk;

                /* Clear B-device session valid state change interrupt flag */
                OTG_CLR_INT_FLAG(OTG_INTSTS_BVLDCHGIF_Msk);
                printf("break-B (OTG_STATUS: 0x%x)\n", OTG->STATUS);
            }
        }
        else     /* A-device */
        {
            s_u8IsAdevice = 1;
            printf("A-device (OTG_STATUS: 0x%x)\n", OTG->STATUS);
            /* Clear ID status changed interrupt flag */
            OTG_CLR_INT_FLAG(OTG_INTSTS_IDCHGIF_Msk);
            /* Enable ID status changed interrupt */
            OTG_ENABLE_INT(OTG_INTEN_IDCHGIEN_Msk);
            USBH_Process();
            /* Disable ID status changed interrupt */
            OTG_DISABLE_INT(OTG_INTEN_IDCHGIEN_Msk);
        }
    }
}
