#include "internal.h"

int handshake(connection_info *con)
{
    return 0;
}

void *load_library(char *name)
{
    void *handle;

    handle = dlopen(name, RTLD_LAZY);
    if(!handle)
    {
        fprintf(stderr, "%s\n", dlerror());
        return NULL;
    }

    return handle;
}

void *load_symbol(void *lib, char *name)
{
    void *symbol;
    char *error;

    symbol = dlsym(lib, name);
    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "%s\n", error);
        return NULL;
    }

    return symbol;
}

int close_library(void *lib)
{
    if(lib == NULL){ return 1; }
    else{ return dlclose(lib); }
}
