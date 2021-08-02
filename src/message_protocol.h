#ifndef MESSAGE_PROTOCOL
#define MESSAGE_PROTOCOL

#define BYTE char
#define MSG_CONTENTS_SZ 2046

/**
 * Structure for storing the type and contents of a message
 */
struct message {
    short message_type;
    BYTE message_contents[MSG_CONTENTS_SZ];
};

// helper methods for setting up message structs
short get_message_type(BYTE buffer[]);
void initialise_message(struct message *msg, BYTE buffer[]);

// helper methods for processing messages
void parse_connect_message(BYTE buffer[], BYTE domain_identity_path[],
BYTE identifier[], BYTE domain[]);
void get_pipe_names(BYTE domain_identity_path[], BYTE read_pipe[], BYTE write_pipe[]);
void create_directory(BYTE domain[]);

#endif