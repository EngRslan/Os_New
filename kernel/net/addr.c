#include <kernel/net/addr.h>
#include <stdio.h>




void ip2str(string_t str,ipv4_addr_t * ip){
    sprintf(str,"%d.%d.%d.%d",ip->n[0],ip->n[1],ip->n[2],ip->n[3]);
}
void mac2str(string_t str,eth_addr_t * mac){
    sprintf(str,"%x:%x:%x:%x:%x:%x",mac->n[0],mac->n[1],mac->n[2],mac->n[3],mac->n[4],mac->n[5]);
}