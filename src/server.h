#ifndef SERVER_H
#define SERVER_H
#include <signal.h>
#include <dirent.h>
#include <sys/select.h>

void signal_handler(int signum, siginfo_t *siginfo, void *context);

#endif
