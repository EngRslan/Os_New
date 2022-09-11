#include <kernel/net/addr.h>
#include <stdio.h>




void ip2str(string_t str,ipv4_addr_t * ip){
    sprintf(str,"%d.%d.%d.%d",(uint32_t)ip->n[0],(uint32_t)ip->n[1],(uint32_t)ip->n[2],(uint32_t)ip->n[3]);
}
void mac2str(string_t str,eth_addr_t * mac){
    sprintf(str,"%x:%x:%x:%x:%x:%x",(uint32_t)mac->n[0],(uint32_t)mac->n[1],(uint32_t)mac->n[2],(uint32_t)mac->n[3],(uint32_t)mac->n[4],(uint32_t)mac->n[5]);
}