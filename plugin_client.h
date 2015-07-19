#ifndef PLUGIN_H
#define PLUGIN_H

#include "shared.h"

/**
 * @brief run_client
 * Defines interface for client side library function
 * @param con Containter to all server which are connected
 */
void run_client(network_info *con);

#endif // PLUGIN_H
