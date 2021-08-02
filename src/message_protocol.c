#include "message_protocol.h"
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MSG_TYPE_SZ 2
#define CONNECT_SEGMENT_SZ 256
#define DOMAIN_IDENTITY_PATH_SZ 524

/**
 * Returns the decimal value of the message
 */
short get_message_type(BYTE buffer[]) {
    short msg_value = 0;
    memcpy(&msg_value, buffer, MSG_TYPE_SZ);
    return msg_value;
}

/**
 * Loads a message struct with the relevant data for its type and contents
 */
void initialise_message(struct message *msg, BYTE buffer[]) {
    msg->message_type = get_message_type(buffer);
    memcpy(msg->message_contents, &buffer[MSG_TYPE_SZ], MSG_CONTENTS_SZ);
}

/**
 * Gets the RD and WR pipe names for the client by appending relevant suffix
 * to the file path
 */
void get_pipe_names(BYTE domain_identity_path[], BYTE read_pipe[], BYTE write_pipe[]) {

    strcpy(read_pipe, domain_identity_path);
    strcpy(write_pipe, domain_identity_path);
    strcat(read_pipe, "_RD");
    strcat(write_pipe, "_WR");

}

/**
* Gets the identifier, domain, and file path from the CONNECT message
*/
void parse_connect_message(BYTE buffer[], BYTE domain_identity_path[],
BYTE identifier[], BYTE domain[]) {

    // make sure strings are null terminated
    identifier[CONNECT_SEGMENT_SZ - 1] = '\0';
    domain_identity_path[DOMAIN_IDENTITY_PATH_SZ - 1] = '\0';
    domain[CONNECT_SEGMENT_SZ - 1] = '\0';

    // setup ptr at third byte
    BYTE *buffer_ptr = &buffer[MSG_TYPE_SZ];
    memcpy(identifier, buffer_ptr, CONNECT_SEGMENT_SZ);

    // move ptr done 256 bytes to next segment
    buffer_ptr += CONNECT_SEGMENT_SZ;
    memcpy(domain_identity_path, buffer_ptr, CONNECT_SEGMENT_SZ);
    memcpy(domain, buffer_ptr, CONNECT_SEGMENT_SZ);

    // create directory @ domain before it is modified to include identity
    create_directory(domain_identity_path);

    strcat(domain_identity_path, "/");
    strcat(domain_identity_path, identifier);

}


/**
 * Creates a directory for the domain if it does not yet exist
 */
void create_directory(BYTE domain[]) {

    DIR* dir = opendir(domain);

    if (dir) {
        closedir(dir);
        return;
    } else {
        mkdir(domain, 0777);
    }

}