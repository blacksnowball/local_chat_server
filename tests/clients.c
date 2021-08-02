#include "clients.h"
#include <signal.h>
#include <dirent.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MSG_SIZE 2048
#define CONNECT_COMPONENT_SZ 256
#define TYPE_SZ 2

void msg_code_set(char buffer[], int msg_code) {
    memcpy(buffer, &msg_code, 2);
}

void setup_connect_msg(char buffer[], char *domain, char *identifier, int
msg_code) {
    msg_code_set(buffer, msg_code);
    memcpy(&buffer[TYPE_SZ], identifier, CONNECT_COMPONENT_SZ);
    memcpy(&buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], domain, CONNECT_COMPONENT_SZ);
}

void setup_say_msg(char buffer[], char *msg, int msg_code) {
    msg_code_set(buffer, msg_code);
    memcpy(&buffer[TYPE_SZ], msg, 1790);
}

void setup_saycont_msg(char buffer[], char *msg, int msg_code, int null_byte) {
    msg_code_set(buffer, msg_code);
    memcpy(&buffer[TYPE_SZ], msg, 1789);
    memcpy(&buffer[MSG_SIZE-1], &null_byte, 1);
}