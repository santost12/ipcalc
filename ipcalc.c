#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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

/*
Valid subnet masks should have consecutive '1' bits
followed by consecutive '0' bits.
*/
bool valid_subnet_mask(struct ipv4 *subnet_mask)
{
    /* combine the octets to 32 bit unsigned int */
    uint32_t mask = ((uint32_t)subnet_mask->octet_1 << 24) |
                    ((uint32_t)subnet_mask->octet_2 << 16) |
                    ((uint32_t)subnet_mask->octet_3 << 8) |
                    (uint32_t)subnet_mask->octet_4;

    bool consecutive_zeros = false;

    for (int i = 31; i >= 0; i--) {
        if (mask & (1 << i)) {
            if (consecutive_zeros) {
                return false;
            }
        } else {
            consecutive_zeros = true;
        }
    }

    return true;
}

/*
Check if valid network address by using the bitwise AND operation between
the network address and the inverted subnet mask.
*/
bool valid_network_addr(struct ipv4 *network_addr, struct ipv4 *subnet_mask)
{
    struct ipv4 calculated_network_addr;

    calculated_network_addr.octet_1 = network_addr->octet_1 & subnet_mask->octet_1;
    calculated_network_addr.octet_2 = network_addr->octet_2 & subnet_mask->octet_2;
    calculated_network_addr.octet_3 = network_addr->octet_3 & subnet_mask->octet_3;
    calculated_network_addr.octet_4 = network_addr->octet_4 & subnet_mask->octet_4;

    bool is_valid = 
        calculated_network_addr.octet_1 == network_addr->octet_1 &&
        calculated_network_addr.octet_2 == network_addr->octet_2 &&
        calculated_network_addr.octet_3 == network_addr->octet_3 &&
        calculated_network_addr.octet_4 == network_addr->octet_4;

    return is_valid;
}


int main(int argc, char *argv[])
{
/*
    if (argc < 3) {
        printf("Error: Too few arguments.\n");
        printf("Example: ipcalc 192.168.0.0 255.255.255.0\n");
        return -1;
    } else if (argc > 3) {
        printf("Error: Too many arguments.\n");
        printf("Example: ipcalc 192.168.0.0 255.255.255.0\n");
        return -1;
    }

    printf("[debug] First argument is: %s\n", argv[1]);
    printf("[debug] Second argument is: %s\n\n", argv[2]);
*/

    struct ipv4 network_addr;
    struct ipv4 subnet_mask;
    struct ipv4 broadcast_addr;
    struct ipv4 first_usable_addr;
    struct ipv4 last_usable_addr;


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

/*
    We need to check if the subnet mask is valid before
    we can check if the network address is valid.    
*/
    if (valid_subnet_mask(&subnet_mask) == true) {
        calculate_broadcast(&network_addr, &subnet_mask, &broadcast_addr);
    } else {
        printf("Error: Invalid subnet mask\n");
        return -2;
    }


    if (valid_network_addr(&network_addr, &subnet_mask) == true) {
        calculate_usable_ip(&network_addr, &broadcast_addr, &first_usable_addr, &last_usable_addr);
    } else {
        printf("Error: Invalid network address for the subnet mask you provided\n");
        return -3;
    }

    printf("Network address: %u.%u.%u.%u\n", network_addr.octet_1, network_addr.octet_2, network_addr.octet_3, network_addr.octet_4);
    printf("Subnet mask: %u.%u.%u.%u\n", subnet_mask.octet_1, subnet_mask.octet_2, subnet_mask.octet_3, subnet_mask.octet_4);
    printf("Broadcast address: %u.%u.%u.%u\n", broadcast_addr.octet_1, broadcast_addr.octet_2, broadcast_addr.octet_3, broadcast_addr.octet_4);
    printf("Usable range: %u.%u.%u.%u - %u.%u.%u.%u\n", first_usable_addr.octet_1, first_usable_addr.octet_2, first_usable_addr.octet_3, first_usable_addr.octet_4, last_usable_addr.octet_1, last_usable_addr.octet_2, last_usable_addr.octet_3, last_usable_addr.octet_4);
    return 0;
}
