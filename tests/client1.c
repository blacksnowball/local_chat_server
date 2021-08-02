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
    char client3_buffer[MSG_SIZE];
    char output[2100];
    output[2099] = '\0';

    mkfifo(GEVENT, 0666);
    int fd_server = open(GEVENT, O_RDWR);

    /**
     * Testing CONNECT from multiple clients
     */

    char domain[CONNECT_COMPONENT_SZ] = "foo";
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

    // open fd
    int fd_client1[2] = {open("foo/bar_RD", O_RDWR), open("foo/bar_WR",
                                                          O_RDWR)};
    int fd_client2[2] = {open("foo/zoo_RD", O_RDWR), open("foo/zoo_WR",
                                                          O_RDWR)};
    int fd_client3[2] = {open("foo/breq_RD", O_RDWR), open("foo/breq_WR",
                                                           O_RDWR)};

    /**
     * Testing ordinary functionality of SAY/RECV
     */

    char msg_say_normal_1[1790] = "Hello, friend.";
    char msg_say_normal_2[1790] = "Where is Jessica Hyde?";
    char msg_say_normal_3[1790] = "I have no idea.";
    char msg_say_normal_4[1790] = "Neither do I. Good day!";
    char msg_say_normal_5[1790] = "Late to the party!";

    setup_say_msg(client1_buffer, msg_say_normal_1, SAY_MSG_CODE);
    write(fd_client1[1], client1_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client2[0], client2_buffer, MSG_SIZE);
    memcpy(output, &client2_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    output[1790] = '\0';
    printf("Received MSG from identifier %s who says: %s\n",
           &client2_buffer[TYPE_SZ], output);

    setup_say_msg(client2_buffer, msg_say_normal_2, SAY_MSG_CODE);
    write(fd_client2[1], client2_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client1[0], client1_buffer, MSG_SIZE);
    memcpy(output, &client1_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    printf("Received MSG from identifier %s who says: %s\n",
           &client1_buffer[TYPE_SZ], output);

    setup_say_msg(client1_buffer, msg_say_normal_3, SAY_MSG_CODE);
    write(fd_client1[1], client1_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client2[0], client2_buffer, MSG_SIZE);
    memcpy(output, &client2_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    printf("Received MSG from identifier %s who says: %s\n",
           &client2_buffer[TYPE_SZ], output);

    setup_say_msg(client2_buffer, msg_say_normal_4, SAY_MSG_CODE);
    write(fd_client2[1], client2_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client1[0], client1_buffer, MSG_SIZE);
    memcpy(output, &client1_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    printf("Received MSG from identifier %s who says: %s\n",
           &client1_buffer[TYPE_SZ], output);

    setup_say_msg(client3_buffer, msg_say_normal_5, SAY_MSG_CODE);
    write(fd_client3[1], client3_buffer, MSG_SIZE);
    sleep(2);
    read(fd_client1[0], client1_buffer, MSG_SIZE);
    memcpy(output, &client1_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    printf("Received MSG from identifier %s who says: %s\n",
           &client1_buffer[TYPE_SZ], output);
    read(fd_client2[0], client2_buffer, MSG_SIZE);
    memcpy(output, &client2_buffer[TYPE_SZ+CONNECT_COMPONENT_SZ], 1790);
    printf("Received MSG from identifier %s who says: %s\n",
           &client2_buffer[TYPE_SZ], output);


    close(fd_server);

}
