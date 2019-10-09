#define WIN32_LEAN_AND_MEAN
#define THREAD LPTHREAD_START_ROUTINE

#undef UNICODE

#include <stdio.h>   //debug
#include <stdlib.h>  //
#include <windows.h> //winapi

#include <ws2tcpip.h>

#include <winsock2.h>

#include "rapi.cpp"

#include "socket.cpp"
#include "http_parse.cpp" 