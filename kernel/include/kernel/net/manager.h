#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include <kernel/net/addr.h>

void NetworkInstall();
NetInterface *GetDefaultInterface();

#endif