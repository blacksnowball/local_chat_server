#ifndef MESSAGE_PROCESS
#define MESSAGE_PROCESS

#include "message_protocol.h"
#include <unistd.h>
#define BYTE char

void prepare_directory_filename(BYTE file_path[], BYTE domain[],
                                int *file_break_index_ptr);
void disconnect_client_handler(BYTE file_path[], BYTE domain[],
                               int *file_break_index_ptr, pid_t parent_pid);
void process_say_msg_as_recv(BYTE *say_message, BYTE *receive_msg,
                             BYTE *client_handler_buffer, BYTE *file_path,
                             BYTE *domain, int *file_break_index_ptr,
                             BYTE *identifier, struct message *client_msg_ptr);
void process_saycont_msg_as_recvcont(BYTE *say_message, BYTE *receive_msg,
                                     BYTE *client_handler_buffer, BYTE *file_path,
                                     BYTE *domain, int *file_break_index_ptr,
                                     BYTE *identifier,
                                     struct message *client_message_ptr);
void prepare_transmission(int msg_type, BYTE receive_msg[], BYTE
say_message[], BYTE identifier[]);
void distribute_msg_in_domain(BYTE domain[], BYTE file_path[], BYTE
identifier[], BYTE receive_msg[], int *file_break_index_ptr);

#endif
