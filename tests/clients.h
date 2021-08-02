#ifndef CLIENTS
#define CLIENTS

void msg_code_set(char buffer[], int msg_code);
void setup_connect_msg(char buffer[], char *domain, char *identifier, int
msg_code);
void setup_say_msg(char buffer[], char *msg, int msg_code);
void setup_saycont_msg(char buffer[], char *msg, int msg_code, int null_byte);

#endif
