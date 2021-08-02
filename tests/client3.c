#include "clients.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define GEVENT "gevent"
#define MSG_SIZE 2048

#define CONNECT_COMPONENT_SZ 256
#define TYPE_SZ 2
#define CONNECT_MSG_CODE 0
#define SAY_MSG_CODE 1
#define SAYCONT_MSG_CODE 2
#define DISCONNECT_MSG_CODE 7


int main() {

    char client1_buffer[MSG_SIZE];
    char client2_buffer[MSG_SIZE];
    char client3_buffer[MSG_SIZE];

    mkfifo(GEVENT, 0666);
    int fd_server = open(GEVENT, O_RDWR);

    /**
     * Testing CONNECT from multiple clients
     */

    char domain[CONNECT_COMPONENT_SZ] = "dell";
    char client1[CONNECT_COMPONENT_SZ] = "bar";
    char client2[CONNECT_COMPONENT_SZ] = "zoo";
    char client3[CONNECT_COMPONENT_SZ] = "breq";

    setup_connect_msg(client1_buffer, domain, client1, CONNECT_MSG_CODE);
    write(fd_server, client1_buffer, MSG_SIZE);
    sleep(2);
    setup_connect_msg(client2_buffer, domain, client2, CONNECT_MSG_CODE);
    write(fd_server, client2_buffer, MSG_SIZE);
    sleep(2);
    setup_connect_msg(client3_buffer, domain, client3, CONNECT_MSG_CODE);
    write(fd_server, client3_buffer, MSG_SIZE);
    sleep(2);
    // open fd
    int fd_client1[2] = {open("dell/bar_RD", O_RDWR), open("dell/bar_WR",
                                                           O_RDWR)};
    int fd_client2[2] = {open("dell/zoo_RD", O_RDWR), open("dell/zoo_WR",
                                                           O_RDWR)};
    int fd_client3[2] = {open("dell/breq_RD", O_RDWR), open("dell/breq_WR",
                                                            O_RDWR)};


    /**
     * Testing transmission of messages that do not comply with the protocol
     * for the global and client process including positive and negatve
     * values. These should prompt error messages for both processes.
     */
    char random_msg[1790] = "This should not be processed!";
    char random_msg2[1789] = "This should not be processed!";

    setup_connect_msg(client1_buffer, domain, random_msg, CONNECT_MSG_CODE);
    msg_code_set(client1_buffer, -11);
    write(fd_server, client1_buffer, MSG_SIZE);
    sleep(2);

    setup_say_msg(client2_buffer, random_msg,
                  431);
    msg_code_set(client2_buffer, 18);
    write(fd_client2[1], client2_buffer, MSG_SIZE);
    sleep(2);

    setup_saycont_msg(client3_buffer, random_msg2, SAYCONT_MSG_CODE, 255);
    msg_code_set(client3_buffer, -55);
    write(fd_client3[1], client3_buffer, MSG_SIZE);
    sleep(2);

    /**
    * Testing disconnect of client handlers
    */

    msg_code_set(client1_buffer, DISCONNECT_MSG_CODE);
    write(fd_client1[1], client1_buffer, MSG_SIZE);
    sleep(2);
    msg_code_set(client2_buffer, DISCONNECT_MSG_CODE);
    write(fd_client2[1], client1_buffer, MSG_SIZE);

}

