#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


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
            consecutive_zeros = false;
        } else {
            consecutive_zeros = true;
        }
    }

    return consecutive_zeros;
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


void ipv4_to_binary(const char *ipv4_str, struct ipv4 *ipv4_addr) {
    char temp_str[16];
    strncpy(temp_str, ipv4_str, sizeof(temp_str));
    temp_str[sizeof(temp_str) - 1] = '\0';

    char *token = strtok(temp_str, ".");
    ipv4_addr->octet_1 = atoi(token);

    for (int i = 2; i <= 4; i++) {
        token = strtok(NULL, ".");
        if (token == NULL) {
            printf("Error: Invalid IPv4 address format.\n");
            exit(-1);
        }
        if (i == 2) {
            ipv4_addr->octet_2 = atoi(token);
        } else if (i == 3) {
            ipv4_addr->octet_3 = atoi(token);
        } else if (i == 4) {
            ipv4_addr->octet_4 = atoi(token);
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s network_id subnet_mask\n", argv[0]);
        return -1;
    }

    struct ipv4 network_addr;
    struct ipv4 subnet_mask;
    struct ipv4 broadcast_addr;
    struct ipv4 first_usable_addr;
    struct ipv4 last_usable_addr;

    ipv4_to_binary(argv[1], &network_addr);
    ipv4_to_binary(argv[2], &subnet_mask);

/*
    We need to check if the subnet mask is valid before
    we can check if the network address is valid.
*/
    if (valid_subnet_mask(&subnet_mask) != true) {
        printf("Error: Invalid subnet mask\n");
        return -2;
    }

    if (valid_network_addr(&network_addr, &subnet_mask) != true) {
        printf("Error: Invalid network address\n");
        return -3;
    }

    calculate_usable_ip(&network_addr, &broadcast_addr, &first_usable_addr, &last_usable_addr);
    calculate_broadcast(&network_addr, &subnet_mask, &broadcast_addr);

    printf("Network address: %u.%u.%u.%u\n", network_addr.octet_1, network_addr.octet_2, network_addr.octet_3, network_addr.octet_4);
    printf("Subnet mask: %u.%u.%u.%u\n", subnet_mask.octet_1, subnet_mask.octet_2, subnet_mask.octet_3, subnet_mask.octet_4);
    printf("Broadcast address: %u.%u.%u.%u\n", broadcast_addr.octet_1, broadcast_addr.octet_2, broadcast_addr.octet_3, broadcast_addr.octet_4);
    printf("Usable range: %u.%u.%u.%u - %u.%u.%u.%u\n", first_usable_addr.octet_1, first_usable_addr.octet_2, first_usable_addr.octet_3, first_usable_addr.octet_4, last_usable_addr.octet_1, last_usable_addr.octet_2, last_usable_addr.octet_3, last_usable_addr.octet_4);
    return 0;
}
