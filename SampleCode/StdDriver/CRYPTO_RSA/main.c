/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Shows how to use Crypto RSA engine to sign and verify
 *           signatures.
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#define RSA_BIT_LEN      2048

__ALIGNED(4) static char    N[RSA_KBUF_HLEN] = "bad47a84c1782e4dbdd913f2a261fc8b65838412c6e45a2068ed6d7f16e9cdf4462b39119563cafb74b9cbf25cfd544bdae23bff0ebe7f6441042b7e109b9a8afaa056821ef8efaab219d21d6763484785622d918d395a2a31f2ece8385a8131e5ff143314a82e21afd713bae817cc0ee3514d4839007ccb55d68409c97a18ab62fa6f9f89b3f94a2777c47d6136775a56a9a0127f682470bef831fbec4bcd7b5095a7823fd70745d37d1bf72b63c4b1b4a3d0581e74bf9ade93cc46148617553931a79d92e9e488ef47223ee6f6c061884b13c9065b591139de13c1ea2927491ed00fb793cd68f463f5f64baa53916b46c818ab99706557a1c2d50d232577d1";
__ALIGNED(4) static char    E[RSA_KBUF_HLEN] = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001";
__ALIGNED(4) static char    d[RSA_KBUF_HLEN] = "40d60f24b61d76783d3bb1dc00b55f96a2a686f59b3750fdb15c40251c370c65cada222673811bc6b305ed7c90ffcb3abdddc8336612ff13b42a75cb7c88fb936291b523d80acce5a0842c724ed85a1393faf3d470bda8083fa84dc5f31499844f0c7c1e93fb1f734a5a29fb31a35c8a0822455f1c850a49e8629714ec6a2657efe75ec1ca6e62f9a3756c9b20b4855bdc9a3ab58c43d8af85b837a7fd15aa1149c119cfe960c05a9d4cea69c9fb6a897145674882bf57241d77c054dc4c94e8349d376296137eb421686159cb878d15d171eda8692834afc871988f203fc822c5dcee7f6c48df663ea3dc755e7dc06aebd41d05f1ca2891e2679783244d068f";
__ALIGNED(4) static char    Msg[RSA_KBUF_HLEN] = "70992c9d95a4908d2a94b3ab9fa1cd643f120e326f9d7808af50cac42c4b0b4eeb7f0d4df303a568fbfb82b0f58300d25357645721bb71861caf81b27a56082c80a146499fb4eab5bde4493f5d00f1a437bbc360dfcd8056fe6be10e608adb30b6c2f7652428b8d32d362945982a46585d2102ef7995a8ba6e8ad8fd16bd7ae8f53c3d7fcfba290b57ce7f8f09c828d6f2d3ce56f131bd9461e5667e5b73edac77f504dac4f202a9570eb4515b2bf516407db831518db8a2083ec701e8fd387c430bb1a72deca5b49d429cf9deb09cc4518dc5f57c089aa2d3420e567e732102c2c92b88a07c69d70917140ab3823c63f312d3f11fa87ba29da3c7224b4fb4bc";

//#define RSA_BIT_LEN         4096
//#define RSA_KEY_SIZE_IDX    RSA_KEY_SIZE_4096
//__ALIGNED(4) static char    N[RSA_KBUF_HLEN] = "922e538bc840bc00b0ef09d138ece912ff77de07bdc1cd7cf9543eb720abc13816021a6a25926ccc1ae75008fa23847a3c518a1d27492e02028dd2592fb978e497e4fd6d886bc0b68697fd41b88d88518de81bbfc06d50702972ba80eaf041d6d87508333aa868f195c9dbaced76733152697839c6a93b634ec4e558ea0affce728367822275e669c00760c20154503202b7d2bab27043e0242a82b45c3ef2b13ccf192821faec2d2c367a5c61dd9d458441bf3fdf5f0229024c909a8df6e22b1a5b0a213b4cfdb0838dccd4c527510bfb1a9c3434d767e11b79fd3cf611ea3818cc392575989d1a63da685f1a3498fa9d73902d2271a6cb32c1dcba4974fb8f708ab2c744c03d8f439058587c2273a09777bbcc72901a3c574644a70806f20e889871e9b301fb7d2231cfcd6453d3f7065c8b32cb7af1682ed5ca7f9bcef7305393b86eb925365865c760e7baf1bcdfef3a63ff009d3a7096fa75f3e2a7295c17fe1e14e2f8ebee89827bc56d9e1bf2c4e28f0206e321e1de61e103da2ae3f4b7b83ecea7c213b8e8d9f205cf364e8f9d4cd3332fdc9085266ad0287c9a0c6341e1eb87e26d4a2fa9f9d5817bc843676ce738ecaea00b767c9772c6c6768cd841dc4302500df209347b2f5aeb86762aaa4ff9cbebd63d412682b294941201b9f2b760bd42291e66a5e37df61e1269edc422aa922706250b1562a8ac5b1b4745";
//__ALIGNED(4) static char    E[RSA_KBUF_HLEN] = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001";
//__ALIGNED(4) static char    d[RSA_KBUF_HLEN] = "6c017a8d1f0a655e055cb85e33b7278677e6f2cfb96e860b3e94f6398c141b10fb81d64e73303ade9b4a292258df0134e7783f99c6bb15193b15da433735cc92e97fb197ca6202e35992d6f6460b9c9ad54497ac17db2cc1927e483a945babe9b7c8d0a43944d1351e3d24e82213908cf29d26cd597114860d5f012daecf2dae3581cd6aa3578544c133325465730c45c72856b4c6be9807f5f0caddc402668279b1729eea6c61300728c1439cb3874e23a8084c8317c99acf2e0ef182ced4d0900a8ff379c32af5ce5d68e8143b3670a9b9418c79366674363eedd2aae515af18f35b8dc7d358d074d29ca717cfa0ef663a58c3bd2805c29ac17143901ad09ed5f216dbe79b2dd0a0409f71a129bf86954c8643b593db9bb36f5f5344c89ea4de576a8e423833a91a2167693ac53fdad40ced4ab56ac71b97dec459fd8c26f0a19bca2e0b12d9c44201bad30d345361ddc46fb7f5da1e0e1ebe0c2e48d843cc782e8789c485ac8e6a0fefe12e66e7907f11e4ea658506f1694e9fa46acd004b5181700b42952fc08c51f50bc64ca06ea455b8ce75df80847b3ce9c1db2b7d9c0bf1328117dafc3027fe4c8355c235ad5a9194e4a42d1978a2f2e5ba7b3909cdc030f69e91c7d9f52f9f25bc1e000559d9b788252824c09a70cbf51acf25b76fc4e3b5018480eb548c450a3e1bbfb64c22fdb7209102b716a64fab09283cff49";
//__ALIGNED(4) static char    Msg[RSA_KBUF_HLEN] = "70992c9d95a4908d2a94b3ab9fa1cd643f120e326f9d7808af50cac42c4b0b4eeb7f0d4df303a568fbfb82b0f58300d25357645721bb71861caf81b27a56082c80a146499fb4eab5bde4493f5d00f1a437bbc360dfcd8056fe6be10e608adb30b6c2f7652428b8d32d362945982a46585d2102ef7995a8ba6e8ad8fd16bd7ae870992c9d95a4908d2a94b3ab9fa1cd643f120e326f9d7808af50cac42c4b0b4eeb7f0d4df303a568fbfb82b0f58300d25357645721bb71861caf81b27a56082c80a146499fb4eab5bde4493f5d00f1a437bbc360dfcd8056fe6be10e608adb30b6c2f7652428b8d32d362945982a46585d2102ef7995a8ba6e8ad8fd16bd7ae870992c9d95a4908d2a94b3ab9fa1cd643f120e326f9d7808af50cac42c4b0b4eeb7f0d4df303a568fbfb82b0f58300d25357645721bb71861caf81b27a56082c80a146499fb4eab5bde4493f5d00f1a437bbc360dfcd8056fe6be10e608adb30b6c2f7652428b8d32d362945982a46585d2102ef7995a8ba6e8ad8fd16bd7ae870992c9d95a4908d2a94b3ab9fa1cd643f120e326f9d7808af50cac42c4b0b4eeb7f0d4df303a568fbfb82b0f58300d25357645721bb71861caf81b27a56082c80a146499fb4eab5bde4493f5d00f1a437bbc360dfcd8056fe6be10e608adb30b6c2f7652428b8d32d362945982a46585d2102ef7995a8ba6e8ad8fd16bd7ae8";



static RSA_BUF_NORMAL_T s_sRSABuf;

static volatile int g_RSA_done;
static volatile int g_RSA_error;

void CRYPTO_IRQHandler(void);
void SYS_Init(void);
void DEBUG_PORT_Init(void);


void CRYPTO_IRQHandler(void)
{
    if(PRNG_GET_INT_FLAG(CRYPTO))
    {
        PRNG_CLR_INT_FLAG(CRYPTO);
    }
    if(SHA_GET_INT_FLAG(CRYPTO))
    {
        SHA_CLR_INT_FLAG(CRYPTO);
    }
    if(RSA_GET_INT_FLAG(CRYPTO))
    {
        g_RSA_done = 1;
        if(RSA_GET_INT_FLAG(CRYPTO)&CRYPTO_INTSTS_RSAEIF_Msk)
        {
            g_RSA_error = 1;
            printf("RSA error flag is set!!\n");
        }
        RSA_CLR_INT_FLAG(CRYPTO);
    }
}

void SYS_Init(void)
{

      /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

   /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  
    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);


   /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);    

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);
    
    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(4);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable CRYPTO module clock */
    CLK_EnableModuleClock(CRYPTO0_MODULE);

		 /* Debug UART clock setting*/
     SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    char    OutputResult[RSA_KBUF_HLEN];
    uint32_t u32TimeOutCnt;
  
    SYS_UnlockReg();

   /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init Debug UART for printf */
    InitDebugUart();
    /* Lock protected registers */
    SYS_LockReg();

    printf("\n\n+---------------------------------------------+\n");
    printf("|   Crypto RSA sample                         |\n");
    printf("+---------------------------------------------+\n");

    NVIC_EnableIRQ(CRYPTO_IRQn);

    RSA_ENABLE_INT(CRYPTO);

    g_RSA_done = 0;
    g_RSA_error = 0;

    printf("Private key (N,d) -\n");
    printf("    N = %s\n", N);
    printf("    d = %s\n", d);
    printf("Public key (N,e) -\n");
    printf("    E = %s\n", E);

    /*---------------------------------------
     *  RSA sign
     *---------------------------------------*/
    /* Configure RSA operation mode and key length */
    if(RSA_Open(CRYPTO, RSA_MODE_NORMAL, RSA_KEY_SIZE_2048, &s_sRSABuf, sizeof(s_sRSABuf), 0) != 0)
    {
        printf("\nRSA buffer size is incorrect!!\n");
        return -1;
    }
    /* Set RSA private key */
    RSA_SetKey(CRYPTO, d);
    RSA_SetDMATransfer(CRYPTO, Msg, N, 0, 0);
    RSA_Start(CRYPTO);

    /* Waiting for RSA operation done */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(!g_RSA_done)
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for RSA operation done time-out!\n");
            return -1;
        }
    }

    /* Check error flag */
    if(g_RSA_error)
    {
        printf("\nRSA has error!!\n");
        return -1;
    }

    /* Get RSA output result */
    RSA_Read(CRYPTO, OutputResult);
    printf("\nRSA sign: %s\n", OutputResult);

    /*---------------------------------------
     *  RSA verify
     *---------------------------------------*/
    g_RSA_done = 0;
    g_RSA_error = 0;

    /* Configure RSA operation mode and key length */
    if(RSA_Open(CRYPTO, RSA_MODE_NORMAL, RSA_KEY_SIZE_2048, &s_sRSABuf, sizeof(s_sRSABuf), 0))
    {
        printf("\nRSA buffer size is incorrect!!\n");
        return -1;
    }
    /* Set RSA public key */
    RSA_SetKey(CRYPTO, E);
    RSA_SetDMATransfer(CRYPTO, OutputResult, N, 0, 0);
    RSA_Start(CRYPTO);

    /* Waiting for RSA operation done */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(!g_RSA_done)
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for RSA operation done time-out!\n");
            return -1;
        }
    }

    /* Check error flag */
    if(g_RSA_error)
    {
        printf("\nRSA has error!!\n");
        return -1;
    }

    /* Get RSA output result */
    RSA_Read(CRYPTO, OutputResult);
    printf("\nRSA Output: %s\n", OutputResult);

    /* Verify the message */
    if(strcasecmp(OutputResult, Msg) == 0)
        printf("\nRSA signature verify OK.\n");
    else
    {
        printf("\nRSA signature verify failed!!\n");
        //return -1;
    }
    printf("\nDone.\n");
    while(1);
}
