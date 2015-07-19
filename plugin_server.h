#ifndef PLUGIN_H
#define PLUGIN_H

#include "shared.h"

/**
 * @brief run_server
 * Defines interface for server side library function
 * @param con Container to client connection
 */
void run_server(connection_info *con);

#endif // PLUGIN_H
