#include "stdio.h"
#include "NuMicro.h"
#if 0
#include "usb_common.h"
#include "usb_pd.h"
#else
#include "utcpdlib.h"  
#endif

utcpd_pvFunPtr utcpd_pvFunPtrTable[CONFIG_USB_PD_PORT_MAX_COUNT] = { 0 };

#if 0
UTCPD_InstallCallback(int32_t port, utcpd_pvFunPtr* pfn); 

callback_function(port, UTCPD_PD_EVENT, op)  

if(utcpd_pvFunPtrTable[port] != 0)          //Callback 
    utcpd_pvFunPtrTable[port](port, EVENT, );
#endif 

                                       
void UTCPD_InstallCallback(int port, utcpd_pvFunPtr* pfn)
{
    utcpd_pvFunPtrTable[port] = (utcpd_pvFunPtr)pfn;
    //utcpd_pvFunPtrTable[port] = pfn;    
}


void UTCPD_NotifyEvent(int port, uint32_t event, uint32_t op)
{
    if(utcpd_pvFunPtrTable[port] != 0)          
        utcpd_pvFunPtrTable[port](port, event, op);
}
  