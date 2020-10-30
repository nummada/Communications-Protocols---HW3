#include "requests.h"
#include <sys/socket.h>
#include "helpers.h"

void getBooksCommand(char* token, char* message, char* response);
int isNumber(char id[COMMAND_LEN]);
void getBookCommand(char* token, char* message, char* response, char* cookie);
void addBookCommand(char* token, char* message, char* response, char* cookie);
void deleteBookCommand(char* cookie, char* message, char* response, char* token);