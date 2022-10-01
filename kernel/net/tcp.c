#include <kernel/net/tcp.h>
#include <kernel/mem/kheap.h>
#include <kernel/net/manager.h>
#include <kernel/net/intf.h>
#include <string.h>
#include <stdbool.h>

TcpConnection *TcpCreate()
{
    TcpConnection *conn = (TcpConnection *)kmalloc(sizeof(TcpConnection));
    memset((void *)conn,0,sizeof(TcpConnection));

    return conn;
}


bool TcpConnect(TcpConnection *conn, const Ipv4Address addr,uint16_t port)
{
    NetInterface *intf = GetDefaultInterface();
    
    conn->localPort = 5000;
    CopyIpv4Address(intf->Ip,conn->localAddr);
    conn->remotePort = port;
    CopyIpv4Address(addr,conn->remotePort);



}