#if !defined(RTL8139)
#define RTL8139
#include <kernel/drivers/pci.h>

void rtl8139_install(pci_config_t * _device);

#endif // RTL8139
