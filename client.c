#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, calloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "bookHelpers.h"

//functie care inregistreaza un client
void registerCommand(char *response, char *message){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	char username[COMMAND_LEN];
	char password[COMMAND_LEN];
	memset(username,0, COMMAND_LEN);
	memset(password, 0, COMMAND_LEN);

	//preiau input  si parsez
	printf("username=");
	fgets(username, COMMAND_LEN, stdin);
	username[strlen(username) - 1] = '\0';
	printf("password=");
	fgets(password, COMMAND_LEN, stdin);
	password[strlen(password) - 1] = '\0';

	//creez payload
	char data [100];
	sprintf(data, "{\"username\": \"%s\", \"password\": \"%s\"}",
		username, password);
	char** body = calloc(1, sizeof(char*));
	body[0] = data;

	//creez mesaj si trimit
	message = compute_post_request(host, "/api/v1/tema/auth/register",
		"application/json", body, 1, NULL, 0, NULL);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);
	close(sockfd);
	free(message);
	free(body);
}

//functie care da log in unui client
char* loginCommand(char *response, char *message, int* logged){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	char username[COMMAND_LEN];
	char password[COMMAND_LEN];
	memset(username,0, COMMAND_LEN);
	memset(password, 0, COMMAND_LEN);

	//preiau input si parsez
	printf("username=");
	fgets(username, COMMAND_LEN, stdin);
	username[strlen(username) - 1] = '\0';
	printf("password=");
	fgets(password, COMMAND_LEN, stdin);
	password[strlen(password) - 1] = '\0';
	char data [100];

	//creez payload
	sprintf(data, "{\"username\": \"%s\", \"password\": \"%s\"}",
		username, password);
	char** body = calloc(1, sizeof(char*));
	body[0] = data;

	//creez mesaj si trimit
	message = compute_post_request(host, "/api/v1/tema/auth/login",
		"application/json", body, 1, NULL, 0, NULL);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);

	//daca am primit eroare ma opresc
	char* error = strstr(response, "\"error\"");
	if(error != NULL) return NULL;
	error = strstr(response, "Too many");
	if(error != NULL) return NULL;
	char* cookie = strstr(response, "Set-Cookie");
	cookie = strtok(cookie, "\r\n");
	cookie = cookie + 12;
	close(sockfd);
	*logged = 1;
	free(message);
	free(body);
	return cookie;
}

//functie care cere permisiuni pentru biblioteca
void* enterLibraryCommand(char* message, char* response, char* cookie, int logged){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	char** cookies = calloc(1, sizeof(char*));
	if(logged == 1){
		cookies[0] = cookie;
	}
	//creez mesaj si trimit
	message = compute_get_request(host, "/api/v1/tema/library/access", NULL,
		cookies, 1, NULL);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);

	char* error = strstr(response, "Too many");
	if(error != NULL) return NULL;
	
	//creez token doar daca sunt logat
	char *token;
	if(logged == 1){
		token = strstr(response, "{\"token\":\"");
		token = strtok(token, "}");
		token = token + 10;
		token[strlen(token) - 1] = '\0';
	}
	free(message);
	free(cookies);
	close(sockfd);
	return token;
}

//functie pentru logout
void logoutCommand(int* logged, char* cookie, char* message, char* response){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	char** cookies = calloc(1, sizeof(char*));
	if(*logged == 1){
		*logged = 0;
		//adaug cookie
		cookies[0] = cookie;
		//trimit mesaj
		message = compute_get_request(host, "/api/v1/tema/auth/logout", NULL,
			cookies, 1, NULL);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		//afisez raspuns
		printf("%s\n", response);
	}else{
		printf("You are not logged in.\n");
	}
	close(sockfd);
}


int main(int argc, char *argv[]) {
	int logged = 0;
	char *message = NULL;
	char *response = NULL;
	int fdmax;
	char buffer[COMMAND_LEN];

	char* token = NULL;

	char *cookie = NULL;

	fd_set read_fds;
	fd_set tmp_fds;

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	FD_SET(0, &read_fds);
	fdmax = 0;


	//cat timp primesc comenzi de la stdin
	while(1){
		tmp_fds = read_fds;
		int n = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(n < 0, "select");
		if (FD_ISSET(0, &tmp_fds)){
			memset(buffer, 0, COMMAND_LEN);
			fgets(buffer, BUFLEN, stdin);
			char* command = strtok(buffer, " \n");
			//verific ce comanda primesc si apelez functia specifica comenzii
			if(strcmp(command, "register") == 0){
				registerCommand(response, message);
			}else if(strcmp(command, "login") == 0){
				if(logged == 0){
					cookie = loginCommand(response, message, &logged);
				}else{
					printf("Already logged in. Please log out.\n");
				}
			}else if(strcmp(command, "enter_library") == 0){
				token = enterLibraryCommand(message, response, cookie, logged);
			}else if(strcmp(command, "get_books") == 0){
				getBooksCommand(token, message, response);
			}else if(strcmp(command, "get_book") == 0){
				getBookCommand(token, message, response, cookie);
			}else if(strcmp(command, "add_book") == 0){
				addBookCommand(token, message, response, cookie);
			}else if(strcmp(command, "delete_book") == 0){
				deleteBookCommand(cookie, message, response, token);
			}else if(strcmp(command, "logout") == 0){
				logoutCommand(&logged, cookie, message, response);
				token = NULL;
				cookie = NULL;
			}else if(strcmp(command, "exit") == 0){
				return 0;
			}else{
				printf("Please enter a valid command.\n");
			}
		}
	}

	return 0;
}
