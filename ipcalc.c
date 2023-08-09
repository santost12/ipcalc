#include <stdio.h>
#include <stdint.h>

struct ipv4 {
    uint8_t octet_1;
    uint8_t octet_2;
    uint8_t octet_3;
    uint8_t octet_4;
};

/*
1) perform bitwise NOT operation on subnet mask
2) perform bitwise OR operation on inverted subnet mask and IP
*/
void calculate_broadcast(struct ipv4 *network_addr, struct ipv4 *subnet_mask, struct ipv4 *broadcast_addr)
{
    broadcast_addr -> octet_1 = ~subnet_mask -> octet_1 | network_addr -> octet_1;
    broadcast_addr -> octet_2 = ~subnet_mask -> octet_2 | network_addr -> octet_2;
    broadcast_addr -> octet_3 = ~subnet_mask -> octet_3 | network_addr -> octet_3;
    broadcast_addr -> octet_4 = ~subnet_mask -> octet_4 | network_addr -> octet_4;
}

/*
The first usable IP address is the network address + 1,
and the last usable IP address is the broadcast address - 1. 
*/
void calculate_usable_ip(struct ipv4 *network_addr, struct ipv4 *broadcast_addr, struct ipv4 *first_usable_addr, struct ipv4 *last_usable_addr)
{
    first_usable_addr -> octet_1 = network_addr -> octet_1;
    first_usable_addr -> octet_2 = network_addr -> octet_2;
    first_usable_addr -> octet_3 = network_addr -> octet_3;
    first_usable_addr -> octet_4 = network_addr -> octet_4 + 0b00000001;

    last_usable_addr -> octet_1 = broadcast_addr -> octet_1;
    last_usable_addr -> octet_2 = broadcast_addr -> octet_2;
    last_usable_addr -> octet_3 = broadcast_addr -> octet_3;
    last_usable_addr -> octet_4 = broadcast_addr -> octet_4 - 0b00000001;

    if (last_usable_addr->octet_4 < 0b00000001) {
        last_usable_addr->octet_3--;
        last_usable_addr->octet_4 = 0b11111111 + last_usable_addr->octet_4;
    }
    
    if (last_usable_addr->octet_3 < 0b00000000) {
        last_usable_addr->octet_2--;
        last_usable_addr->octet_3 = 0b11111111 + last_usable_addr->octet_3;
    }
    
    if (last_usable_addr->octet_2 < 0b00000000) {
        last_usable_addr->octet_1--;
        last_usable_addr->octet_2 = 0b11111111 + last_usable_addr->octet_2;
    }
}

int main(void)
{
    struct ipv4 ip_addr;
    struct ipv4 network_addr;
    struct ipv4 subnet_mask;
    struct ipv4 broadcast_addr;
    struct ipv4 first_usable_addr;
    struct ipv4 last_usable_addr;

    /* 192 168 0 1 */
    ip_addr.octet_1 = 0b11000000;
    ip_addr.octet_2 = 0b10101000;
    ip_addr.octet_3 = 0b00000000;
    ip_addr.octet_4 = 0b00000001;

    /* 192 168 0 0 */
    network_addr.octet_1 = 0b11000000;
    network_addr.octet_2 = 0b10101000;
    network_addr.octet_3 = 0b00000000;
    network_addr.octet_4 = 0b00000000;

    /* 255 255 255 0 */
    subnet_mask.octet_1 = 0b11111111;
    subnet_mask.octet_2 = 0b11111111;
    subnet_mask.octet_3 = 0b11111111;
    subnet_mask.octet_4 = 0b00000000;


    calculate_broadcast(&network_addr, &subnet_mask, &broadcast_addr);
    calculate_usable_ip(&network_addr, &broadcast_addr, &first_usable_addr, &last_usable_addr);

    printf("IP address: %u.%u.%u.%u\n", ip_addr.octet_1, ip_addr.octet_2, ip_addr.octet_3, ip_addr.octet_4);
    printf("Network address: %u.%u.%u.%u\n", network_addr.octet_1, network_addr.octet_2, network_addr.octet_3, network_addr.octet_4);
    printf("Subnet mask: %u.%u.%u.%u\n", subnet_mask.octet_1, subnet_mask.octet_2, subnet_mask.octet_3, subnet_mask.octet_4);
    printf("Broadcast address: %u.%u.%u.%u\n", broadcast_addr.octet_1, broadcast_addr.octet_2, broadcast_addr.octet_3, broadcast_addr.octet_4);
    printf("Usable range: %u.%u.%u.%u - %u.%u.%u.%u\n", first_usable_addr.octet_1, first_usable_addr.octet_2, first_usable_addr.octet_3, first_usable_addr.octet_4, last_usable_addr.octet_1, last_usable_addr.octet_2, last_usable_addr.octet_3, last_usable_addr.octet_4);
    return 0;
}