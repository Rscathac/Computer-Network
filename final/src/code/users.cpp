#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/time.h>
#include<string.h>
#include<time.h>
#include<netinet/in.h>
#include<pthread.h>
#include<errno.h>
#include<math.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<algorithm>
#include<iostream>
#include<string>
#include"getpass.h"
#include"tic-tac-toe.h"

#define USERNAME_MAX 32
#define PASSWORD_MAX 32
#define STRING_MAX	 32
#define IP_MAX      128
#define FILE_MAX   1024
#define CMD_EXIT     -1
#define CMD_LOGIN     1
#define CMD_SU        2
#define CMD_CHAT	  3
#define CMD_LOG		  4
#define CMD_FILE      5
#define CMD_CHATTING  6
#define CMD_GAME      7
#define CMD_GAME_REQ  8
#define CMD_GAME_RES  9
#define CMD_FILE_REQ  10
#define CMD_FILE_RES  11
#define CMD_FRIEND    12
#define CMD_ADDFRIEND 13
#define CMD_UNFRIEND  14
#define CMD_LOGOUT	  15
#define CMD_OFFLINE   16
#define LOGIN_SUCCESS 1
#define LOGIN_FAILED  0
#define SU_SUCCESS    1
#define SU_FAILED     0
#define STATUS_CHAT   1
#define STATUS_IDLE   2
#define STATUS_GAME   3
#define USER_ONLINE   1
#define USER_OFFLINE  2
#define USER_INVALID  3
#define FRIEND_OK      1
#define FRIEND_ALREADY 2
#define FRIEND_INVALID 3
#define RECV_MESSAGE  1
#define RECV_GAME     2
#define RECV_GAME_REQ 3
#define RECV_FILE     4
#define RECV_OFFLINE  5
#define RECV_PEOPLEONLINE 6
#define RECV_PEOPLEOFFLINE 7
using namespace std;

void Check_emoji(char input[STRING_MAX], int print) {
	FILE* fp = fopen("emoji.txt", "r");
	char text[STRING_MAX] = "";
	char emoji[STRING_MAX] = "";
	if(print) 
		printf("\033[0;35;49m*******************\033[0m\n");
	

	while(fscanf(fp,"%s%s",text, emoji) == 2) {
		if(print) 
			printf("\033[0;35;49m%s: %s\033[0m\n",text, emoji);
		
		else if(strcmp(input, text) == 0) {
			memset(input, '\0', STRING_MAX);
			strcpy(input, emoji);
			break;
		}
	}
	if(print)
		printf("\033[0;35;49m*******************\033[0m\n");
	fclose(fp);
	return;
}

void hostname_to_ip(char *hostname, char *ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    
    if((he = gethostbyname(hostname)) == NULL) {
        puts("get hostname failed");
        return;
    }
    
    addr_list = (struct in_addr **)he->h_addr_list;
    
    for(int i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return;
    }
    return;
    
}

void Check_operation(int Status) {

	FILE* fp;

	if(Status == STATUS_IDLE) 
		fp = fopen("operation.txt", "r");
	else if(Status == STATUS_CHAT) 
		fp = fopen("chat_operation.txt", "r");
		
	char line[STRING_MAX] = "";
	while(fgets(line, STRING_MAX, fp) != 0) {
		printf("%s", line);
	}
	
	return;
}



int main(int argc, char* argv[]) {

	/* Build up socket with the server */
	int socket_fd, max_fd;
	struct sockaddr_in addr;
	int port = 8700;

	char hostname[IP_MAX] = "";
	char ip[IP_MAX] = "";
	FILE* ip_fp = fopen("config.cfg", "r");
	fscanf(ip_fp, "%s", hostname);
	hostname_to_ip(hostname, ip);
	
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_fd < 0) {
		puts("Open socket failed");
		return -1;
	}

	if(connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		puts("Connect failed");
		return -1;
	}

	printf("\033[1;35;49m+--------------------------------+\033[0m\n");
	printf("\033[1;35;49m|    Welcome to the messenger    |\033[0m\n");
	printf("\033[1;35;49m|    ------------------------    |\033[0m\n");
	printf("\033[1;35;49m|         Login: /login          |\033[0m\n");
	printf("\033[1;35;49m|         Sign up: /su           |\033[0m\n");
	printf("\033[1;35;49m|         Exit:  /exit           |\033[0m\n");
	printf("\033[1;35;49m+--------------------------------+\033[0m\n");
	char command[64] = "";
	int cmd = 0;

	/* Command handle */
	while(scanf("%s", command) != EOF) {
		if(strcmp(command, "/login") == 0) {
			cmd = CMD_LOGIN;
			break;
		}

		else if(strcmp(command, "/su") == 0) {
			cmd = CMD_SU;
			int su = 0;
			while(!su) {
				char su_usr[32] = "";
				char su_pw1[32] = "";
				char su_pw2[32] = "";
				printf("Enter a username:");
				scanf("%s", su_usr);
				while(1) {
					printf("Enter a password:");
					char *p1 = su_pw1;
					getpasswd(&p1, PASSWORD_MAX, '*', stdin);
//					scanf("%s", su_pw1);
					cout << endl;
					printf("Password check:");
					char *p2 = su_pw2;
//					scanf("%s", su_pw2);
					getpasswd(&p2, PASSWORD_MAX, '*', stdin);
					cout << endl;
					if(strcmp(su_pw1, su_pw2) == 0) {
						break;
					}
					else {
						printf("\033[1;31;49mIncorrect password\033[0m\n");
						memset(su_pw1, '\0', sizeof(su_pw1));
						memset(su_pw2, '\0', sizeof(su_pw2));
					}
				}
	
				send(socket_fd, &cmd, sizeof(cmd), 0);
				send(socket_fd, su_usr, strlen(su_usr), 0);
//				printf("usr = %s\n", su_usr);
				sleep(1);
				send(socket_fd, su_pw1, strlen(su_pw1), 0);
//				printf("pw = %s\n", su_pw1);
				int res = 0;
				recv(socket_fd, &res, sizeof(int), 0);
				if(res == 0) {
					printf("This username has been used, try another\n");
				}
				else {
					printf("Sign up Success\n");
					su = 1;
				}
			
			}
		}

		else if(strcmp(command, "/exit") == 0) {
			puts("Exiting the messenger...");
			return -1;
		}
		
		else {
			puts("Invalid command");
		}
		memset(command, '\0', sizeof(command));

	}
	
	char username[USERNAME_MAX] = "";
	char password[PASSWORD_MAX] = "";

	
	int login = 0;
	/* Login Process */

	if(cmd == CMD_LOGIN) {
		while(!login) {
			memset(username, '\0', sizeof(username));
			memset(password, '\0', sizeof(password));
			send(socket_fd, &cmd, sizeof(cmd), 0);
			printf("Username:");
			scanf("%s", username);
			send(socket_fd, username, strlen(username), 0);
			printf("Password:");
			char *p = password;
//			scanf("%s", password);
			getpasswd(&p, PASSWORD_MAX, '*', stdin);
			cout << endl;
			send(socket_fd, password, strlen(password), 0);

			int login_status;
			recv(socket_fd, &login_status, sizeof(int), 0);
			if(login_status == LOGIN_SUCCESS) {
				login = 1;
				printf("\033[5;31;49m*********** Login Success *********\033[0m\n");
			}
			else 
				printf("\033[1;31;49mIncorrect username or password, please enter again\033[0m\n");	
		}	
	}

	fd_set read_fds;
	int Status = STATUS_IDLE;
	int Status_rec = USER_OFFLINE;

	int board[9] = {0};
	int chess = 0;
	int turn = 0;

	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(socket_fd, &read_fds);
		FD_SET(fileno(stdin), &read_fds);
		max_fd = max(socket_fd, fileno(stdin));

		struct timeval tv;
		tv.tv_sec = 100000;
		tv.tv_usec = 0;

		if(select(max_fd+1, &read_fds, NULL, NULL, &tv) < 0)
			continue;
		
		/* Receive message from the server */
		if(FD_ISSET(socket_fd, &read_fds)) {
			
			int ret = 0;
			char message[STRING_MAX] = "";
			char input[STRING_MAX] = "";
			char filename[STRING_MAX] = "";
			char long_message[FILE_MAX] = "";
			int numbytes = 0;
			int flag = 0;
			recv(socket_fd, &ret, sizeof(int), 0);

			/* New message */
			if(ret == RECV_MESSAGE) {
				if(recv(socket_fd, &message, sizeof(message),0) > 0)
					printf("\033[1;95;49m%s\033[0m\n", message);
			}

			/* Game moverment */
			else if(ret == RECV_GAME) {  
				if(recv(socket_fd, &chess, sizeof(int), 0) > 0) {
					board[chess] = 2;
					int ret = tic_tac_toe(board, chess);
					if(ret == 2) { 
						printf("\033[1;35;49mYou lose !!!!!!!!\033[0m\n");
						memset(board, 0, sizeof(board));
						Status = STATUS_CHAT;
					}
					else if(ret == -1) {
						printf("\033[1;35;49mIt's a draw\033[0m\n");
						memset(board, 0, sizeof(board));
						Status = STATUS_CHAT;
					}

					else {
						printf("\n*******************\n");
						printf("Your turn\n");
						turn = 1;
					}
				}
			}
			/* offline message */
			else if(ret == RECV_OFFLINE){
				int numbytes = 0;
				char tmp[STRING_MAX] = "";
				printf("*******************\n");
				while(1){
					memset(tmp, 0, sizeof(tmp)); 
					recv(socket_fd, &numbytes, sizeof(numbytes), 0);
					if(numbytes == -1)
						break;
					else {
						recv(socket_fd, tmp, numbytes, 0);
						fwrite(tmp, sizeof(char), numbytes, stdout);
					}
				}
				printf("*******************\n");
			}

			else if(ret == RECV_PEOPLEONLINE){
				printf("\033[1;35;49mThe users go online\033[0m\n");
				Status_rec = USER_ONLINE;
			}
			
			else if(ret == RECV_PEOPLEOFFLINE){
				printf("\033[1;35;49mThe users logged out\033[0m\n");
				Status_rec = USER_OFFLINE;
			}

			/* Game request */
			else if(ret == RECV_GAME_REQ) {
				printf("\033[1;35;49mDo you want to play tic-tac-toe? [y/n]\033[0m");
				char response[STRING_MAX] = "";
				while(scanf("%s", response)!=EOF){
					if(strcmp(response, "y") != 0 && strcmp(response, "n") != 0) {
						printf("\033[1;31;49minvalid response\033[0m\n");
						printf("\033[1;35;49mDo you want to play tic-tac-toe? [y/n]\033[0m");
						strcpy(response, "\0");
					}	
					else {
						int res = 0;
						if(response[0] == 'y')
							res = 1;
						else 
							res = 0;
						int command = CMD_GAME_RES;
						send(socket_fd, &command, sizeof(command), 0);
						send(socket_fd, &res, sizeof(res), 0);
						if(res == 1) {
							Status = STATUS_GAME;
							printf("\033[1;35;49mGame start\033[0m\n");
							printf("+-----------+\n| 1 | 2 | 3 |\n+-----------+\n| 4 | 5 | 6 |\n+-----------+\n| 7 | 8 | 9 |\n+-----------+\n");
							printf("     You <O>\n");
							printf("Opponent <X>\n");
							printf("\nOpponent's turn\n\n");
						}
						else {
							printf("\033[1;35;49mReject the game\033[0m\n");
						}
						break;
					}
				}
			}
			
			/* Send file operation from server */
			else if(ret == RECV_FILE) {
				recv(socket_fd, &filename, sizeof(filename), 0);
				printf("\033[1;35;49mDo you want to receive the file %s? [y/n]\033[0m", filename);

				while(1) {
					fgets(input, STRING_MAX, stdin);
					input[strlen(input)-1] = '\0';
					if(strcmp(input, "y") == 0) {
						flag = 1;
						int command = CMD_FILE_RES;
						send(socket_fd, &command, sizeof(command), 0);
						send(socket_fd, &flag, sizeof(flag), 0);
						break;
					}

					else if(strcmp(input ,"n") == 0) {
						flag = -1;
						int command = CMD_FILE_RES;
						send(socket_fd, &command, sizeof(command), 0);
						send(socket_fd, &flag, sizeof(flag), 0);
						break;
					}

					else {
						printf("\033[1;35;49mInvalid response\033[0m\n");
						printf("\033[1;35;49mDo you want to receive the file %s? [y/n]\033[0m", filename);
					}
				}

				if(flag == 1) {
					char dirname[STRING_MAX*2] = "";
					strcat(dirname, "src/");
					strcat(dirname, filename);
					FILE *fp = fopen(dirname, "wb");
					int tmp = 0;
					int ret = 0;
					while(1) {
						recv(socket_fd, &numbytes, sizeof(int), 0);
						if(numbytes == 0)
							break;
						else {
							strcpy(long_message, "\0");
							tmp = recv(socket_fd, long_message, numbytes, 0);
							if(tmp != numbytes){
								ret = -1;
								send(socket_fd, &ret, sizeof(ret), 0);
								continue;
							}
							else{
								ret = 1;
								send(socket_fd, &ret, sizeof(ret), 0);
								fwrite(long_message, sizeof(char), numbytes, fp);
							}
						}
					}

					printf("\033[1;35;49mReceive file success\033[0m\n");
					fclose(fp);
				}
				else if(flag == -1) 
					printf("\033[1;35;49mReject the file\033[0m\n");				
			}
		}

		/* Read command line input */
		if(FD_ISSET(fileno(stdin), &read_fds)) {
			char input[STRING_MAX];
			string input_str;
			fgets(input, STRING_MAX, stdin);				// fgets will read '\n' of input
			input[strlen(input)-1] = '\0';					// remove the trailing '\n' of input
			if(input[0] == '\0')
				continue;
//			printf("input = %s\n", input);
			/* Handle log operation */
			if(strncmp(input, "/log ", 5) == 0){
				char other[USERNAME_MAX] = {};
				int flag = 0; 					//判斷log是否結束
				cmd = CMD_LOG;
				int numbytes = 0;

				strcpy(other, input+5);
				send(socket_fd, &cmd, sizeof(cmd), 0);
				send(socket_fd, other, strlen(other), 0);
				printf("//////////////////////////////////\n");

				
				while(1){
					char message[STRING_MAX] = "";
					recv(socket_fd, &numbytes, sizeof(numbytes), 0);
					if(numbytes == 0){
						break;
					}
					else {
						recv(socket_fd, message, numbytes, 0);
						fwrite(message, sizeof(char), sizeof(message), stdout);
					}
				}
				printf("//////////////////////////////////\n");
			}

			/* Handle chat operation */
			else if(strncmp(input, "/chat ", 6) == 0){
				char other[USERNAME_MAX] = {};
				cmd = CMD_CHAT;
				strcpy(other, input+6);
				if(strcmp(other,username)==0) {
					printf("\033[1;31;49mYou can't chat with yourself, try other username\033[0m\n");
				}
				else {
					send(socket_fd, &cmd, sizeof(cmd), 0);
					send(socket_fd, other, strlen(other), 0);
					int ret = 0;
					recv(socket_fd, &ret, sizeof(int), 0);
					
					/* The user is online */
					if(ret == USER_ONLINE) {
						Status = STATUS_CHAT;
						Status_rec = USER_ONLINE;
						printf("\033[1;29;49m************* Chatting with %s *************\033[0m\n", other);
					}
					/* The user is offline */
					else if(ret == USER_OFFLINE) {
						Status = STATUS_CHAT;
						Status_rec = USER_OFFLINE;
						printf("\033[1;31;49m******** %s is not online, You can leave a message ********\033[0m\n", other);
					}
					/* The user does not exist */
					else if(ret == USER_INVALID) {
						printf("\033[1;31;49mThe user does not exist\033[0m\n");
					}
				}
			}

			/* Friend operation */
			else if(strcmp(input, "/friend") == 0) {
				cmd = CMD_FRIEND;
				send(socket_fd, &cmd, sizeof(cmd), 0);
				int no_friend = 1;
				while(1) {
					int ret = 0;
					recv(socket_fd, &ret, sizeof(int), 0);
					if(ret == EOF)
						break;
					int online = 0;
					recv(socket_fd, &online, sizeof(int), 0);
					
					if(no_friend) {
						no_friend = 0;
						printf("\033[1;33;49mOnline\033[0m   \033[1;34;49mOffline\033[0m\n**********************\n");
					}
					char friend_message[USERNAME_MAX] = "";
					
					recv(socket_fd, friend_message, sizeof(friend_message), 0);
					if(online == 1)
						printf("\033[1;33;49m%s\033[0m\n", friend_message);
					else if(online == -1)
						printf("\033[1;34;49m%s\033[0m\n", friend_message);
					
					
				}
				if(no_friend) {
					printf("\033[1;31;49mSorry, you don't have a friend. You can use /addfriend to add a new friend\033[0m\n");
				}
			}

			else if(strncmp(input, "/addfriend ", 11) == 0) {
				cmd = CMD_ADDFRIEND;
				char friendname[USERNAME_MAX] = "";
				strcpy(friendname, input+11);
				int ret = 0;
				//printf("friendname = %s\n", friendname);
				send(socket_fd, &cmd, sizeof(cmd), 0);
				send(socket_fd, friendname, strlen(friendname), 0);
				recv(socket_fd, &ret, sizeof(int), 0);
							
				/* Add frined success*/
				if(ret == FRIEND_OK) {
					printf("\033[1;35;49mAdd friend %s success\033[0m\n", friendname);
				}
				/* Add friend failed */
				else if(ret == FRIEND_ALREADY){
					printf("\033[1;31;49m%s is already your friend\033[0m\n", friendname);
				}
				else if(ret == FRIEND_INVALID) {
					printf("\033[1;31;49m%s does not exist\033[0m\n", friendname);
				}
			}

			else if(strncmp(input, "/unfriend ", 10) == 0) {
				cmd = CMD_UNFRIEND;
				char friendname[USERNAME_MAX] = "";
				strcpy(friendname, input+10);
				int ret = 0;
				//printf("friendname = %s\n", friendname);
				send(socket_fd, &cmd, sizeof(cmd), 0);
				send(socket_fd, friendname, strlen(friendname), 0);
				recv(socket_fd, &ret, sizeof(int), 0);
							
				/* Delete frined success*/
				if(ret == FRIEND_OK) {
					printf("\033[1;35;49mUnfriend %s success\033[0m\n", friendname);
				}
				/* Delete friend failed */
				else if(ret == FRIEND_INVALID){
					printf("\033[1;31;49m%s is not your friend\033[0m\n", friendname);
				}
	
			}

			else if(strcmp(input, "/logout") == 0) {
				cmd = CMD_LOGOUT;
				send(socket_fd, &cmd, sizeof(cmd), 0);
				printf("\033[1;35;49mSee you again !\033[0m\n");
				return 0;
			}

			else if(Status == STATUS_IDLE) {
				if(strcmp(input, "/?") == 0) {
					Check_operation(Status);
				}

				else if(strlen(input) != 0) {
					puts("\033[1;31;49mInvalid command\033[0m\n");
				}
				
			}

			else if(Status == STATUS_CHAT && Status_rec == USER_ONLINE) {								
				cmd = CMD_CHATTING;
				char display[USERNAME_MAX+STRING_MAX] = "";

				/* Check operation table */
				if(strcmp(input, "/?") == 0)
					Check_operation(Status);
				
				/* Check emoji table */
				else if(strcmp(input, "/emoji?")==0)
					Check_emoji(input, 1);

				/* Send game request */
				else if(strcmp(input, "/game") == 0) {
					printf("\033[1;35;49mSend game request\033[0m\n");
					cmd = CMD_GAME_REQ;
					int ret = 0;
					send(socket_fd, &cmd, sizeof(cmd), 0);
					recv(socket_fd, &ret, sizeof(int), 0);
					if(ret == -1) {
						Status = STATUS_CHAT;
						printf("\033[1;35;49mGame request rejected\033[0m\n");
					}
					else if(ret == 1) {
						Status = STATUS_GAME;
						printf("+-----------+\n| 1 | 2 | 3 |\n+-----------+\n| 4 | 5 | 6 |\n+-----------+\n| 7 | 8 | 9 |\n+-----------+\n");
						printf("     You <O>\n");
						printf("Opponent <X>\n");
						printf("\nYour turn\n");
						turn = 1;
					}
				}
				
				/* Send file */
				else if(strncmp(input, "/file ", 6) == 0) {
					char *filename;
					int num_file = 0;
					//char filename[STRING_MAX] = "";
					cmd = CMD_FILE_REQ;
					filename = strtok(input, " ");
					filename = strtok(NULL, " ");
					num_file = atoi(filename);
					while(num_file > 0){
						filename = strtok(NULL, " ");
						printf("%s\n", filename);
						int res = 0;
						int numbytes = 0;
						char long_message[FILE_MAX] = "";
						FILE* fp = fopen(filename, "rb");
						int flag = 0;
						if(fp == NULL) {
							printf("\033[1;31;49mThe file %s doesn't exits\n", filename);
						}
						else {
							cmd = CMD_FILE_REQ;
							send(socket_fd, &cmd, sizeof(cmd), 0);
							send(socket_fd, filename, strlen(filename), 0);
							recv(socket_fd, &res, sizeof(int), 0);
							if(res == 1) {
								cmd = CMD_FILE;
								printf("\033[1;35;49mStart sending the file %s\033[0m\n", filename);
								send(socket_fd, &cmd, sizeof(cmd), 0);
								send(socket_fd, filename, strlen(filename), 0);

								while(!feof(fp)) {
									numbytes = fread(long_message, sizeof(char), sizeof(long_message), fp);
//									send(socket_fd, &cmd, sizeof(cmd), 0);
									send(socket_fd, &numbytes, sizeof(numbytes), 0);
									send(socket_fd, long_message, numbytes, 0);

									recv(socket_fd, &flag, sizeof(flag), 0);
									while(flag == -1){
										send(socket_fd, &numbytes, sizeof(numbytes), 0);
										send(socket_fd, long_message, numbytes, 0);
										recv(socket_fd, &flag, sizeof(flag), 0);
									}
									strcpy(long_message, "\0");
								}
								numbytes = 0;
//								send(socket_fd, &cmd, sizeof(cmd), 0);
								send(socket_fd, &numbytes, sizeof(numbytes), 0);
								printf("\033[1;35;49mSend file success %s\033[0m\n", filename);
							}
							else if(res == -1) {
								printf("\033[1;35;49mSend file rejected %s\033[0m\n", filename);
							}
						}
						fclose(fp);
						num_file--;
					}
				}
				else if(strcmp(input, "/exit") == 0){
					cmd = CMD_EXIT;
					send(socket_fd, &cmd, sizeof(cmd), 0);
					Status = STATUS_IDLE;
					printf("\033[1;35;49mExit chatting\033[0m\n");

				}
				else {
					Check_emoji(input, 0);
					strcpy(display, username);
					strcat(display, ": ");
					strcat(display, input);
					send(socket_fd, &cmd, sizeof(cmd), 0);
					send(socket_fd, input, strlen(input), 0);
					printf("\033[1;32;49m%s\033[0m\n", display);
				}
			}
			else if(Status == STATUS_CHAT && Status_rec == USER_OFFLINE) {								
				cmd = CMD_OFFLINE;
				char display[USERNAME_MAX+STRING_MAX] = "";

				/* Check operation table */
				if(strcmp(input, "/?") == 0)
					Check_operation(Status);
				
				/* Check emoji table */
				else if(strcmp(input, "/emoji?")==0)
					Check_emoji(input, 1);
				
				else if(strcmp(input, "/exit") == 0){
					cmd = CMD_EXIT;
					send(socket_fd, &cmd, sizeof(cmd), 0);
					Status = STATUS_IDLE;
					printf("\033[1;35;49mExit chatting\033[0m\n");
				}

				else {
					Check_emoji(input, 0);
					strcpy(display, username);
					strcat(display, ": ");
					strcat(display, input);
					send(socket_fd, &cmd, sizeof(cmd), 0);
					send(socket_fd, input, strlen(input), 0);
					printf("\033[1;32;49m%s\033[0m\n", display);
				}

			}
			else if(Status == STATUS_GAME) {
				cmd = CMD_GAME;
				if(turn != 1) {
					printf("\033[1;31;49mIt's not your turn\033[0m\n");
					continue;
				}
				int chess = atoi(input);
				chess -= 1;
				if(chess < 0 || chess > 8 || board[chess] != 0) 
					printf("\033[1;31;49mInvalid move\033[0m\n");
				else {
					board[chess] = 1;
					int ret = tic_tac_toe(board, chess);
					printf("\n*******************\n\n");
					if(ret == 1) {
						printf("\033[1;35;49mCongrats, you win !!!!!!!!\033[0m\n");
						Status = STATUS_CHAT;
						memset(board, 0, sizeof(board));
					}
					else if(ret == -1) {
						printf("\033[1;35;49mIt's a draw\033[0m\n");
						Status = STATUS_CHAT;
						memset(board, 0, sizeof(board));
					}
					else
						printf("Opponent's turn\n");
					turn = 0;
					send(socket_fd, &cmd, sizeof(cmd), 0);
					send(socket_fd, &chess, sizeof(chess), 0);
				}
			}
		}
		
	}
}
