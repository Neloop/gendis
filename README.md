## Gendis - General distributed system ##

Written as semestral work on "Programming in Unix" course on MFF.

### Description
This program should be some kind of very simple distributed system, which allows server-client architecture of computing stuff. Computing itself is done on servers, clients are providers of jobs.

Jobs are defined through dynamic libraries and have to fulfil `plugin_client.h` and `plugin_server.h` interfaces. That means these libraries have to be available on client and server at the time of computing.

### Compilation
Program is available only on Unix platform and therefore `Makefile` is provided and should be used for compilation.

Licensed under [MIT license](https://opensource.org/licenses/MIT).