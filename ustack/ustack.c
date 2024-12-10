#include <stdio.h>
#include <stdlib.h>
#include <rte_eal.h>
#include <rte_ethdev.h>

int gDpdkdPortId = 0;


int main(int argc, char const *argv[])
{
    if (rte_eal_init(argc, argv) < 0){
        rte_exit(EXIT_FAILURE, "Error with EAL init\n");
    }

    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    if (nb_sys_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Support eth found\n");
    }

    printf("nb_sys_ports:%d\n",nb_sys_ports);

    struct rte_eth_dev_info dev_info;
    

    return 0;
}
