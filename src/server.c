#include "message_protocol.h"
#include "server.h"
#include "message_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/select.h>

#define GEVENT "gevent"
#define BUFFER_SZ 2048
#define TYPE_SZ 2
#define CONNECT_SEGMENT_SZ 256
#define DOMAIN_IDENTITY_PATH_SZ 524
#define MSG_SZ 1790
#define BYTE char

#define CONNECT_MSG_CODE 0
#define SAY_MSG_CODE 1
#define SAYCONT_MSG_CODE 2
#define DISCONNECT_MSG_CODE 7


int main(int argc, char** argv) {

    // setup signal handling
    struct sigaction signal_processor;
    memset(&signal_processor, 0, sizeof(signal_processor));
    signal_processor.sa_sigaction = signal_handler;
    sigaction(SIGUSR1, &signal_processor, NULL);

    // set up char arrays for storing data to be extracted from messages
    BYTE buffer[BUFFER_SZ];
    BYTE domain_identity_path[DOMAIN_IDENTITY_PATH_SZ];
    BYTE identifier[CONNECT_SEGMENT_SZ];
    BYTE domain[CONNECT_SEGMENT_SZ];
    BYTE read_pipe[DOMAIN_IDENTITY_PATH_SZ];
    BYTE write_pipe[DOMAIN_IDENTITY_PATH_SZ];

    // set up file descriptors for global and client processes
    int global_read_fd;
    int client_handler_fd[2];
    int read_bytes;

    // variables for process identifiers
    pid_t pid;
    pid_t parent_pid = getpid();

    // set up struct for parsing messages in global parent process
    struct message global_msg;
    struct message *global_msg_ptr = &global_msg;

    // variables for select
    fd_set global_fd_set;
    struct timeval timeout;
    int select_ret_val;

    // check that we can create gevent and read fd
    mkfifo(GEVENT, 0666);

    if ((global_read_fd = open(GEVENT, O_RDWR)) == -1) {
        fprintf(stderr, "Could not open file descriptor for gevent.\n");
        return 1;
    }

    // global process

    while (1) {

        FD_ZERO(&global_fd_set);
        FD_SET(global_read_fd, &global_fd_set);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        select_ret_val = select(global_read_fd + 1, &global_fd_set, NULL, NULL,
                                &timeout);

        if (select_ret_val <= 0) {
            continue;
        } else {

            read_bytes = read(global_read_fd, buffer, BUFFER_SZ);

            if (read_bytes == 0) {
                continue;
            }

            // otherwise parse some message and create client-handler
            initialise_message(global_msg_ptr, buffer);

            if (global_msg_ptr->message_type == CONNECT_MSG_CODE) {

                pid = fork();

                if (pid < 0) {
                    fprintf(stderr, "Unable to fork!\n");
                    return 1;
                } else if (pid == 0) {
                    parse_connect_message(buffer, domain_identity_path,
                                          identifier,
                                          domain);
                    get_pipe_names(domain_identity_path, read_pipe, write_pipe);
                    mkfifo(read_pipe, 0666);
                    mkfifo(write_pipe, 0666);
                    client_handler_fd[0] = open(read_pipe, O_RDWR);
                    client_handler_fd[1] = open(write_pipe, O_RDWR);
                    close(client_handler_fd[0]);
                    close(client_handler_fd[1]);
                    break;
                } else if (pid > 0) {
                    continue;
                }

            } else {
                fprintf(stderr,
                "Bad message! Not a valid message type (should only be "
                "CONNECT requests).\n");
            }

        }


    }

    // child process for client handler

    if (pid == 0) {

        // set up char arrays for storing data to be extracted from messages
        BYTE client_handler_buffer[BUFFER_SZ];
        BYTE say_message[MSG_SZ];
        BYTE receive_msg[BUFFER_SZ];
        BYTE file_path[DOMAIN_IDENTITY_PATH_SZ];

        // struct for parsing client messages
        struct message client_msg;
        struct message *client_msg_ptr = &client_msg;

        // variables for navigating files
        int file_break_index;
        int *file_break_index_ptr = &file_break_index;

        fd_set client_fd_set;


        while (1) {

            client_handler_fd[1] = open(write_pipe, O_RDONLY);

            FD_ZERO(&client_fd_set);
            FD_SET(client_handler_fd[1], &client_fd_set);
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            select_ret_val = select(client_handler_fd[1] + 1, &client_fd_set,
            NULL, NULL, &timeout);

            if (select_ret_val <= 0) {
                close(client_handler_fd[1]);
                continue;
            } else {

                read_bytes = read(client_handler_fd[1], client_handler_buffer,
                                  BUFFER_SZ);

                if (read_bytes == 0) {
                    close(client_handler_fd[1]);
                    continue;
                }

                initialise_message(client_msg_ptr, client_handler_buffer);

                if (client_msg_ptr->message_type == DISCONNECT_MSG_CODE) {

                    disconnect_client_handler(file_path, domain,
                                              file_break_index_ptr, parent_pid);

                    kill(parent_pid, SIGUSR1);
                    sleep(3);
                    exit(0);

                } else if (client_msg_ptr->message_type == SAY_MSG_CODE) {

                    process_say_msg_as_recv(say_message, receive_msg,
                                            client_handler_buffer, file_path,
                                            domain, file_break_index_ptr,
                                            identifier, client_msg_ptr);

                } else if (client_msg_ptr->message_type == SAYCONT_MSG_CODE) {

                    process_saycont_msg_as_recvcont(say_message, receive_msg,
                                                    client_handler_buffer,
                                                    file_path,
                                                    domain,
                                                    file_break_index_ptr,
                                                    identifier, client_msg_ptr);

                } else {
                    fprintf(stderr, "Bad message! Not a valid message type.\n");
                }

            }

            close(client_handler_fd[1]);

        }


    }

    if (pid > 0) {
        close(global_read_fd);
    }

    return 0;

}

/**
 * Signal handler to clean up a child process when it sends a SIGUSR1 signal
 * to parent process to indicate it is shutting down
 */
void signal_handler(int signum, siginfo_t *siginfo, void *context) {
    waitpid(siginfo->si_pid, NULL, 0);
}

