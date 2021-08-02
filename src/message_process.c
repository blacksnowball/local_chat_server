#include "message_process.h"
#include "message_protocol.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>

#define TYPE_SZ 2
#define CONNECT_SEGMENT_SZ 256
#define MSG_SZ 1790
#define BUFFER_SZ 2048
#define DOMAIN_IDENTITY_PATH_SZ 524
#define RECEIVE_MSG_CODE 3
#define RECVCONT_MSG_CODE 4


/**
 * Logic for finalising a RECEIVE OR RECVCONT message to be sent
 */
void prepare_transmission(int msg_type, BYTE receive_msg[], BYTE
say_message[], BYTE identifier[]) {
    short receive_type_value = msg_type;
    memcpy(receive_msg, &receive_type_value, TYPE_SZ);
    memcpy(&receive_msg[TYPE_SZ], identifier, CONNECT_SEGMENT_SZ);
    memcpy(&receive_msg[TYPE_SZ + CONNECT_SEGMENT_SZ], say_message, MSG_SZ);
}

/**
 * Sends a RECEIVE or RECVCONT message to all other pipes in the domain
 */
void distribute_msg_in_domain(BYTE domain[], BYTE file_path[], BYTE
identifier[], BYTE receive_msg[], int *file_break_index_ptr) {


    DIR *dir = opendir(domain);
    struct dirent *file;
    prepare_directory_filename(file_path, domain, file_break_index_ptr);

    if (NULL == dir) {
        fprintf(stderr, "NO DIRECTORY TO BE FOUND HERE!\n");
    } else {

        while ((file = readdir(dir))) {

            char *filename = file->d_name;

            if (strstr(filename, identifier) == NULL &&
                strstr(filename, "_RD")) {
                strcpy(&file_path[*file_break_index_ptr], filename);
                int file_read_fd = open(file_path, O_WRONLY);
                write(file_read_fd, receive_msg, BUFFER_SZ);
                close(file_read_fd);
            }


        }

        closedir(dir);

    }

}

/**
 * Logic for parsing and sending a SAYCONT message to be a RECVCONT message
 */
void process_saycont_msg_as_recvcont(BYTE *say_message, BYTE *receive_msg,
BYTE *client_handler_buffer, BYTE *file_path, BYTE *domain, int
*file_break_index_ptr, BYTE *identifier, struct message *client_message_ptr) {

    // copy from BUFFER of SAYCONT message into say_message contents
    memcpy(say_message, client_message_ptr->message_contents, MSG_SZ - 1);
    memcpy(&say_message[MSG_SZ - 1], &client_handler_buffer[BUFFER_SZ - 1], 1);

    // now copy contents over to receive message to be written in WR pipe
    prepare_transmission(RECVCONT_MSG_CODE, receive_msg, say_message,identifier);
    distribute_msg_in_domain(domain, file_path, identifier, receive_msg,
                             file_break_index_ptr);

}


/**
 * Logic for parsing and sending a SAY message to be a RECV message
 */
void process_say_msg_as_recv(BYTE *say_message, BYTE *receive_msg,
BYTE *client_handler_buffer, BYTE *file_path,
BYTE *domain, int *file_break_index_ptr, BYTE *identifier, struct message
*client_msg_ptr) {

    memcpy(say_message, client_msg_ptr->message_contents, MSG_SZ);
    prepare_transmission(RECEIVE_MSG_CODE, receive_msg, say_message,identifier);
    distribute_msg_in_domain(domain, file_path, identifier, receive_msg,
                             file_break_index_ptr);


}

/**
 * Logic for disconnecting a client handler
 */
void disconnect_client_handler(BYTE file_path[], BYTE domain[], int
*file_break_index_ptr, pid_t parent_pid) {

    DIR *dir = opendir(domain);
    struct dirent *file;

    if (NULL == dir) {
        fprintf(stderr, "NO DIRECTORY TO BE FOUND HERE!\n");
    } else {

        prepare_directory_filename(file_path, domain, file_break_index_ptr);

        while ((file = readdir(dir))) {
            char *filename = file->d_name;
            strcpy(&file_path[*file_break_index_ptr], filename);
            remove(file_path);
        }
        closedir(dir);

    }

}


/**
 * Setup a filename for directory navigating
 */
void prepare_directory_filename(BYTE file_path[], BYTE domain[],
int *file_break_index_ptr) {
    file_path[DOMAIN_IDENTITY_PATH_SZ - 1] = '\0';
    memcpy(file_path, domain, CONNECT_SEGMENT_SZ);
    strcat(file_path, "/");
    *file_break_index_ptr = strlen(file_path);
}
