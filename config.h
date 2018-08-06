#ifndef CONFIG_H
#define CONFIG_H


//all the included header file
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>
#include <csignal>

//all the defined port number for the project

#define UDP_SERVER_A 21087
#define UDP_SERVER_B 22087
#define UDP_SERVER_C 23087
#define UDP_AWS 24087
#define TCP_AWS_C 25087
#define TCP_AWS_M 26087
#define BUFF_SIZE 2048

#endif
