#include "bookHelpers.h"
#include "helpers.h"
//functie care cere informatii succinte despre toate cartile
void getBooksCommand(char* token, char* message, char* response){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	//creez mesaj si trimit
	message = compute_get_request(host, "/api/v1/tema/library/books",
		NULL, NULL, 0, token);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);
	close(sockfd);
	free(message);
}

//functie care verifica daca un string e numar
int isNumber(char id[COMMAND_LEN]){
	for(int i = 0 ; i < strlen(id) ; i++){
		if(isdigit(id[i]) == 0) return 0;
	}
	return 1;
}

//functie care cere informatii despre o carte
void getBookCommand(char* token, char* message, char* response, char* cookie){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	char id[COMMAND_LEN];
	memset(id, 0, COMMAND_LEN);
	//preiau input si parsez
	printf("id=");
	fgets(id, COMMAND_LEN, stdin);
	id[strlen(id) - 1] = '\0';
	char* aux = strtok(id, " ");
	strcpy(id, aux);
	//verific daca id-ul este numar si oblig pana cand se introduce un numar
	if(isNumber(id) == 0){
		while(1){
			printf("Please insert a number.\n");
			printf("id=");
			fgets(id, COMMAND_LEN, stdin);
			id[strlen(id) - 1] = '\0';
			char* aux = strtok(id, " ");
			strcpy(id, aux);
			if(isNumber(id) == 1) break;
		}
	}

	//creez url
	char* path = calloc(150, sizeof(char));
	sprintf(path, "/api/v1/tema/library/books/%s", id);
	char** cookies = calloc(1, sizeof(char*));
	cookies[0] = cookie;
	//creez mesaj si trimit
	message = compute_get_request(host, path, NULL, cookies, 1, token);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);
	close(sockfd);
	free(message);
	free(path);
	free(cookies);
}

//functie care adauga o carte pe server
void addBookCommand(char* token, char* message, char* response, char* cookie){
	char host [100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

	//cer si parsez toate informatiile despre carte
	//titlu
	char title[COMMAND_LEN];
	memset(title, 0, COMMAND_LEN);
	printf("title=");
	fgets(title, COMMAND_LEN, stdin);
	title[strlen(title) - 1] = '\0';

	//autor
	char author[COMMAND_LEN];
	memset(author, 0, COMMAND_LEN);
	printf("author=");
	fgets(author, COMMAND_LEN, stdin);
	author[strlen(author) - 1] = '\0';

	//gen
	char genre[COMMAND_LEN];
	memset(genre, 0, COMMAND_LEN);
	printf("genre=");
	fgets(genre, COMMAND_LEN, stdin);
	genre[strlen(genre) - 1] = '\0';

	//editor
	char publisher[COMMAND_LEN];
	memset(publisher, 0, COMMAND_LEN);
	printf("publisher=");
	fgets(publisher, COMMAND_LEN, stdin);
	publisher[strlen(publisher) - 1] = '\0';

	//numarul de pagini
	char pageCount[COMMAND_LEN];
	memset(pageCount, 0, COMMAND_LEN);
	printf("page_count=");
	fgets(pageCount, COMMAND_LEN, stdin);
	pageCount[strlen(pageCount) - 1] = '\0';
	strcpy(pageCount, strtok(pageCount, " "));

	//verific daca pageCount-ul este numar si oblig introducerea unui numar
	if(isNumber(pageCount) == 0){
		while(1){
			printf("Please insert a valid number.\n");
			printf("page_count=");
			fgets(pageCount, COMMAND_LEN, stdin);
			pageCount[strlen(pageCount) - 1] = '\0';
			strcpy(pageCount, strtok(pageCount, " "));
			if(isNumber(pageCount) == 1) break;
		}
	}

	int no = atoi(pageCount);

	//creez payload-ul asa cum s-a specificat
	char payload[500];
	sprintf(payload, "{\"title\": \"%s\", \"author\": \"%s\", \"genre\":"
		" \"%s\",\"page_count\": \"%d\", \"publisher\": \"%s\"}",
		title, author, genre, no, publisher);
	char** body = calloc(1,sizeof(char*));
	body[0] = payload;
	char** cookies = calloc(1, sizeof(char*));
	cookies[0] = cookie;

	//creez mesaj si trimit
	message = compute_post_request(host, "/api/v1/tema/library/books",
		"application/json", body, 1, cookies, 1, token);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);
	free(message);
	free(body);
	free(cookies);
}

//functie care sterge o carte de pe server
void deleteBookCommand(char* cookie, char* message, char* response, char* token){
	char host[100] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

	//preiau input si parsez
	char id[COMMAND_LEN];
	memset(id, 0, COMMAND_LEN);
	printf("id=");
	fgets(id, COMMAND_LEN, stdin);
	id[strlen(id) - 1] = '\0';
	char* aux = strtok(id, " ");
	strcpy(id, aux);

	//verific daca id-ul este numar si oblig pana cand se introduce un numar
	if(isNumber(id) == 0){
		while(1){
			printf("Please insert a valid number.\n");
			printf("id=");
			fgets(id, COMMAND_LEN, stdin);
			id[strlen(id) - 1] = '\0';
			strcpy(id, strtok(id, " "));
			if(isNumber(id) == 1) break;
		}
	}

	//creez url
	char* path = calloc(150, sizeof(char));
	sprintf(path, "/api/v1/tema/library/books/%s", id);
	char** cookies = calloc(1, sizeof(char*));
	cookies[0] = cookie;

	//creez mesajul
	message = compute_delete_request(host, path, NULL, cookies, 1, token);
	send_to_server(sockfd, message);
	response = receive_from_server(sockfd);
	printf("%s\n", response);
	close(sockfd);
	free(message);
	free(path);
	free(cookies);
}