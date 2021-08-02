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

    /**
     * Note the termination characters inserted into char arrays throughout
     * to ensure we can get a proper printing format without garbage values
     */

    char client1_buffer[MSG_SIZE];
    char client2_buffer[MSG_SIZE];
    char output[2100];
    output[2099] = '\0';

    mkfifo(GEVENT, 0666);
    int fd_server = open(GEVENT, O_RDWR);

    /**
     * Testing CONNECT from multiple clients
     */

    char domain[CONNECT_COMPONENT_SZ] = "void";
    char client1[CONNECT_COMPONENT_SZ] = "bar";
    char client2[CONNECT_COMPONENT_SZ] = "zoo";

    setup_connect_msg(client1_buffer, domain, client1, CONNECT_MSG_CODE);
    write(fd_server, client1_buffer, MSG_SIZE);
    sleep(2);
    setup_connect_msg(client2_buffer, domain, client2, CONNECT_MSG_CODE);
    write(fd_server, client2_buffer, MSG_SIZE);


    // open fd
    int fd_client1[2] = {open("void/bar_RD", O_RDWR), open("void/bar_WR",
                                                           O_RDWR)};
    int fd_client2[2] = {open("void/zoo_RD", O_RDWR), open("void/zoo_WR",
                                                           O_RDWR)};

    /**
     * Testing transmission of message where contents are at boundary value of
     * 1790 ASCII bytes for SAY/RECEIVE combo
     */

    msg_code_set(client1_buffer, SAY_MSG_CODE);
    for (int i = 0; i < 1790; i++) {
        client1_buffer[TYPE_SZ + i] = '@';
    }

    write(fd_client1[1], client1_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client2[0], client2_buffer, MSG_SIZE);
    memcpy(output, &client2_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    output[1790] = '\0';
    printf("Received MSG from identifier %s with contents %s\n",
           &client2_buffer[TYPE_SZ], output);


    /**
    * Testing transmission of message where contents are at boundary value of
    * 1789 ASCII bytes for SAYCONT/RECVCONT combo. Also testing instance where
    * first packet contains termination char of value 255.
    */
    msg_code_set(client2_buffer, SAY_MSG_CODE);
    for (int i = 0; i < 1789; i++) {
        client2_buffer[TYPE_SZ + i] = '!';
    }
    unsigned char termination_byte = 255;
    memcpy(&client2_buffer[MSG_SIZE - 1], &termination_byte, 1);
    write(fd_client2[1], client2_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client1[0], client1_buffer, MSG_SIZE);

    memcpy(output, &client1_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1789);
    output[1789] = '\0';

    printf("Received MSG from identifier %s with contents %s\n",
           &client1_buffer[TYPE_SZ], output);


    sleep(2);
    char client2_buffer_new[MSG_SIZE];
    char random_msg[1789] = "Random message!";
    setup_saycont_msg(client2_buffer_new, random_msg, SAYCONT_MSG_CODE, 1);
    write(fd_client2[1], client2_buffer_new, MSG_SIZE);
    sleep(2);
    read(fd_client1[0], client1_buffer, MSG_SIZE);
    sleep(2);

    char random_msg2[1789] = "No more random messages!";
    setup_saycont_msg(client2_buffer_new, random_msg2, SAYCONT_MSG_CODE, 255);
    write(fd_client2[1], client2_buffer_new, MSG_SIZE);
    sleep(2);
    read(fd_client1[0], client1_buffer, MSG_SIZE);


    close(fd_server);

}
