#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<unordered_map>
#include<vector>
#include<string>
#include<iostream>
#include<fstream>


using namespace std;
#define CLIENT_MAX 1024
#define USERNAME_MAX 32
#define PASSWORD_MAX 32
#define FILE_MAX   1024
#define LOGIN_SUCCESS 1
#define LOGIN_FAILED  0
#define SU_SUCCESS    1
#define SU_FAILED     0
#define CMD_LOGIN     1
#define CMD_SU        2
#define CMD_EXIT     -1
#define CMD_CHAT      3
#define CMD_LOG		  4
#define CMD_FILE	  5
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
#define CMD_OFFLINE	  16
#define USER_ONLINE   1
#define USER_OFFLINE  2
#define USER_INVALID  3
#define FRIEND_OK       1
#define FRIEND_ALREADY  2
#define FRIEND_INVALID  3
#define STRING_MAX    32
#define RECV_MESSAGE  1
#define RECV_GAME	  2
#define RECV_GAME_REQ 3
#define RECV_FILE     4
#define RECV_OFFLINE  5
#define RECV_PEOPLEONLINE 6
#define RECV_PEOPLEOFFLINE 7

int Load_user_data(char usr_list[CLIENT_MAX][USERNAME_MAX]) {
	
	FILE* fp = fopen("account.txt", "r");
	char usr_tmp[USERNAME_MAX] = "";
	char pw_tmp[USERNAME_MAX] = "";
	int usr_num = 0;
	while(fscanf(fp, "%s%s", usr_tmp, pw_tmp) == 2) {
		strcpy(usr_list[usr_num], usr_tmp);
		strcpy(usr_tmp, "");
		usr_num++;
	}

	fclose(fp);
	return usr_num;
	
}

int Friend_control(int op, char target[USERNAME_MAX], char username[USERNAME_MAX]) {
	
	
	int exist = 0;
	char filename[2*USERNAME_MAX+5] = "";
	char friendfile[2*USERNAME_MAX+5] = "";
	strcpy(filename, "friend/");
	strcat(filename, username);
	strcat(filename, ".txt");
	strcpy(friendfile, "friend/");
	strcat(friendfile, target);
	strcat(friendfile, ".txt");
	printf("filename in friend control = %s\n", filename);
	printf("target = %s\n", target);
	FILE* fp = fopen(filename, "r");
	char friends[USERNAME_MAX] = "";
	char friend2[USERNAME_MAX] = "";
	while(fscanf(fp, "%s", friends) != EOF) {
		if(strcmp(friends, target) == 0) {
			exist = 1;
			break;
		}
		strcpy(friends, "\0");
	}
	fclose(fp);
	if(!exist) {
		if(op == CMD_ADDFRIEND) {
			FILE* fp1 = fopen(filename, "a");
			FILE* fp3 = fopen(friendfile, "a");
			if(fp1 != SEEK_SET)
				fputs("\n", fp1);
			fputs(target, fp1);
			fclose(fp1);
			if(fp3 != SEEK_SET)
				fputs("\n", fp3);
			fputs(username, fp3);
			fclose(fp3);
			return FRIEND_OK;
		}
		
		else if(op == CMD_UNFRIEND) {
			return FRIEND_INVALID;
		}
	}
	else {
		if(op == CMD_ADDFRIEND) {
			return FRIEND_ALREADY;
		}
		else if(op == CMD_UNFRIEND) {
			FILE* tmp = fopen("temp.txt", "w");
			FILE* tmp2 = fopen("temp2.txt", "w");
			FILE* fp2 = fopen(filename, "r");
			FILE* fp4 = fopen(friendfile, "r");
			strcpy(friends, "\0");
			strcpy(friend2, "\0");
			while(fgets(friends, USERNAME_MAX, fp2)) {
				printf("read file = %s", friends);
				if(friends[strlen(friends)-1] == '\n') {
					if(strncmp(friends, target, strlen(friends)-1) == 0)
						continue;
				}

				else {
					if(strcmp(friends, target) == 0)
						continue;
				}
				fputs(friends, tmp);
			}
			fclose(tmp);
			fclose(fp2);
			remove(filename);
			rename("temp.txt", filename);
			while(fgets(friend2, USERNAME_MAX, fp4)) {
				printf("read file = %s", friend2);
				if(friend2[strlen(friend2)-1] == '\n') {
					if(strncmp(friend2, username, strlen(friend2)-1) == 0)
						continue;
				}

				else {
					if(strcmp(friend2, username) == 0)
						continue;
				}
				fputs(friend2, tmp2);
			}
			fclose(tmp2);
			fclose(fp4);
			remove(friendfile);
			rename("temp2.txt", friendfile);
			return FRIEND_OK;
		}
	}
	return 0;
	
}

void Historical(char username[USERNAME_MAX], char othername[USERNAME_MAX], int fd){

	char filename[2*USERNAME_MAX+5] = "";
	strcpy(filename, "log/");
	strcat(filename, username);
	strcat(filename, "-");
	strcat(filename, othername);
	strcat(filename, ".txt");
	FILE* fp = fopen( filename, "rb");
	char tmp[STRING_MAX];
	char output[20000]={};
	int tmp_int; 							//send所需的int
	int flag = 0;
	int numbytes = 0;
	if(fp == NULL){
		numbytes = 0;
		send(fd, &numbytes, sizeof(numbytes), 0);
		return;
	}
	
	while(!feof(fp)){
		strcpy(tmp,"\0");
		numbytes = fread(tmp, sizeof(char), sizeof(tmp), fp);
		tmp_int = send(fd, &numbytes, sizeof(numbytes), 0);
		tmp_int = send(fd, tmp, numbytes, 0);
	}
	
	numbytes = 0;
	tmp_int = send(fd, &numbytes, sizeof(numbytes), 0);
	fclose(fp);
	return;

}	

void Transfer_file(int sender_fd, int receiver_fd, char filename[STRING_MAX]){

	int numbytes = 0;
	char long_message[FILE_MAX] = "";
	char tmp_filename[STRING_MAX+5] = "";
	strcpy(tmp_filename, "file/");
	strcat(tmp_filename, filename);
	FILE *fd = fopen(tmp_filename, "wb");
	int tmp = 0;
	int ret = 0;
	while(1) {
		strcpy(long_message,"\0");
		tmp = recv(sender_fd, &numbytes, sizeof(int), 0);
		if(numbytes == 0) 
			break;
		
		tmp = recv(sender_fd, &long_message, numbytes, 0);
		if(tmp != numbytes){
			ret = -1;
			send(sender_fd, &ret, sizeof(ret), 0);
			continue;
		}
		else{
			printf("sending\n");
			ret = 1;
			send(sender_fd, &ret, sizeof(ret), 0);
			fwrite(long_message, sizeof(char), numbytes, fd);
		}
	}
	fclose(fd);
	
	fd = fopen(tmp_filename, "rb");
	while(!feof(fd)) {
		strcpy(long_message,"\0");
		numbytes = fread(long_message, sizeof(char), sizeof(long_message), fd);
		send(receiver_fd, &numbytes, sizeof(numbytes), 0);
		send(receiver_fd, long_message, numbytes, 0);
		recv(receiver_fd, &ret, sizeof(ret), 0 );
		while(ret == -1){
			send(receiver_fd, &numbytes, sizeof(numbytes), 0);
			send(receiver_fd, long_message, numbytes, 0);
			recv(receiver_fd, &ret, sizeof(ret), 0 );
		}
	}
	numbytes = 0;
	send(receiver_fd, &numbytes, sizeof(numbytes), 0);
	fclose(fd);



	return;
}	

void Message_file( char sender[USERNAME_MAX], char receiever[USERNAME_MAX], char message[STRING_MAX]){    
    //sender-receiever
    char filename[2*USERNAME_MAX+5];
	strcpy(filename, "log/");
    strcat(filename, sender);
    strcat(filename, "-");
    strcat(filename, receiever);
    strcat(filename, ".txt");
    FILE* fd = fopen( filename, "a");
    char tmp[STRING_MAX+USERNAME_MAX+2]="";
    strcat(tmp, message);
    strcat(tmp, "\n");
    fputs(tmp, fd);
    fclose(fd);
    //receiever-sender
    char filename2[2*USERNAME_MAX+5];
	strcpy(filename2, "log/");
    strcat(filename2, receiever);
    strcat(filename2, "-");
    strcat(filename2, sender);
    strcat(filename2, ".txt");
    fd = fopen( filename2, "a");
    char tmp2[STRING_MAX+USERNAME_MAX+2]="";
    strcat(tmp2, message);
    strcat(tmp2, "\n");
    fputs(tmp2, fd);
    fclose(fd);
    
}

void Offline_file( char sender[USERNAME_MAX], char receiever[USERNAME_MAX], char message[STRING_MAX]){
    
    //sender-receiever
    char filename[2*USERNAME_MAX+5];
	strcpy(filename, "offline/");
	strcat(filename, receiever);
    strcat(filename, ".txt");
	printf("%s\n", filename);
    FILE* fd = fopen( filename, "a");
    char tmp[STRING_MAX+USERNAME_MAX+2]="";
    strcat(tmp, message);
    strcat(tmp, "\n");
	printf("%s", tmp);
    fputs(tmp, fd);
    fclose(fd);
    
}


int Login(char username[USERNAME_MAX], char password[PASSWORD_MAX]) {
	
	FILE* fp = fopen("account.txt" ,"r");
	char usr[USERNAME_MAX];
	char pw[PASSWORD_MAX];
	while(fscanf(fp, "%s%s", usr, pw) == 2) {
		if(strcmp(usr, username) == 0 && strcmp(pw, password) == 0) {
			return LOGIN_SUCCESS;
		}
	}
			
	return LOGIN_FAILED;
}	

int Sign_up(char username[USERNAME_MAX], char password[PASSWORD_MAX]) {
	
	FILE* fp = fopen("account.txt", "r");
	char usr[USERNAME_MAX];
	char pw[PASSWORD_MAX];
	int flag = 0;
	puts(username);
	puts(password);
	while(fscanf(fp, "%s%s",usr, pw) == 2) {
		flag = 1;
		if(strcmp(usr, username) == 0) {
			puts("existed username");
			return SU_FAILED;
		}
	}

	fclose(fp);
	fp = fopen("account.txt", "a");

	int ret = 0;
	
	if(flag)
		fputs("\n", fp);
	fputs(username, fp);
	fputs(" ", fp);
	fputs(password, fp);

	fclose(fp);

	/* create friend list file */
	char filename[USERNAME_MAX*2] = "";
	strcpy(filename, "friend/");
	strcat(filename, username);
	strcat(filename, ".txt");
	FILE* friend_file = fopen(filename ,"w");
	fclose(friend_file);

	return SU_SUCCESS;

}

int main() {
	
	/* Data initialization */
	int socket_fd, max_fd, new_fd;
	int client_fd[CLIENT_MAX] = {0};
	char usr_list[CLIENT_MAX][USERNAME_MAX];
	int login[CLIENT_MAX] = {0};
	int port = 8700;
	struct sockaddr_in server_addr, client_addr;
	fd_set read_fds, write_fds;

	unordered_map <string, int > usr_to_fd;
	unordered_map <int, string> fd_to_usr;
	unordered_map <int, string> fd_to_name;

	int chat[CLIENT_MAX] = {0};

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		puts("Open socket failed");
		return -1;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	int addr_len = sizeof(server_addr);
	
	if(bind(socket_fd, (struct sockaddr *)&server_addr, addr_len) < 0) {
		puts("Bind failed");
		return -1;
	}

	listen(socket_fd, 8);

	int usr_num = Load_user_data(usr_list);

	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(socket_fd, &read_fds);
		max_fd = socket_fd;

		for(int i = 0; i < CLIENT_MAX; i++) {
			if(client_fd[i] > 0)
				FD_SET(client_fd[i], &read_fds);
			if(client_fd[i] > max_fd)
				max_fd = client_fd[i];
		}

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		if(select(max_fd+1, &read_fds, NULL, NULL, &tv) < 0)
			continue;

		/*  New connection */
		if(FD_ISSET(socket_fd, &read_fds)) {
			new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
			if(new_fd < 0)
				puts("accept failed");

			printf("New user connection [%s:%d]\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));

			for(int i = 0; i < CLIENT_MAX; i++) {
				if(client_fd[i] == 0) {
					client_fd[i] = new_fd;
					break;
				}
			}


		}

		/* User operation handle */
		for(int i = 0; i < CLIENT_MAX; i++) {
			if(FD_ISSET(client_fd[i], &read_fds)) {
				int cmd;
				recv(client_fd[i], &cmd, sizeof(int), 0);

				/* Handle login request */
				if(cmd == CMD_LOGIN) {
					printf("cmd = %d\n", cmd);
					char usr[USERNAME_MAX] = "";
					char pw[PASSWORD_MAX] = "";
					recv(client_fd[i], &usr, sizeof(usr), 0);
					//send();
					printf("usr = %s, pw =%s\n", usr, pw);
					recv(client_fd[i], &pw, sizeof(pw), 0);
					printf("usr = %s, pw =%s\n", usr, pw);
					int login_status = Login(usr, pw);
					send(client_fd[i], &login_status, sizeof(login_status), 0);
					if(login_status == LOGIN_SUCCESS) {
						puts("Login Success");
						string usr_tmp = usr;
						usr_to_fd[usr_tmp] = client_fd[i];
						fd_to_usr[client_fd[i]] = usr_tmp;
						login[client_fd[i]] = 1;
						printf("Map:(%d,%s)\n", usr_to_fd[usr_tmp], fd_to_usr[client_fd[i]].c_str());
						 
						
						char filename[2*STRING_MAX] = "";
						strcpy(filename, "offline/");
						strcat(filename, usr);
						strcat(filename, ".txt");
						FILE *fd = fopen(filename, "rb");
						if(fd != NULL){
							char tmp[STRING_MAX] = "";
							int numbytes = 0;
							int tmp_int = 0;
							int ret = RECV_OFFLINE;
							send(client_fd[i], &ret, sizeof(ret), 0);
							while(!feof(fd)){
								numbytes = fread(tmp, sizeof(char), sizeof(tmp), fd);
								tmp_int = send(client_fd[i], &numbytes, sizeof(numbytes), 0);
								tmp_int = send(client_fd[i], tmp, numbytes, 0);
							}
							numbytes = -1;
							tmp_int = send(client_fd[i], &numbytes, sizeof(numbytes), 0);
							fclose(fd);
							remove(filename);
						}
						
						unordered_map<int, string>::iterator iter;
						for( iter = fd_to_name.begin(); iter != fd_to_name.end();iter++){
							if(strcmp(iter->second.c_str(), usr)==0){
								chat[iter->first] = client_fd[i];
								chat[client_fd[i]] = iter->first;
								fd_to_name.erase(iter);
								int ret = RECV_PEOPLEONLINE;
								send(iter->first, &ret, sizeof(ret), 0);
								break;
							}
						}
					}
					else {
						puts("Login failed");
					}
				}
				
				/* Handle sign up request */
				else if(cmd == CMD_SU) {
					printf("cmd = %d\n", cmd);
					char usr[USERNAME_MAX] = "";
					char pw[PASSWORD_MAX] = "";
					recv(client_fd[i], &usr, sizeof(usr), 0);
					//send();
					printf("usr = %s, pw = %s\n", usr, pw);
					recv(client_fd[i], &pw, sizeof(pw), 0);
					printf("usr = %s, pw = %s\n", usr, pw);
					int su_status = Sign_up(usr, pw);
					send(client_fd[i], &su_status, sizeof(su_status), 0);
					if(su_status == SU_SUCCESS) {
						strcpy(usr_list[usr_num], usr);
						usr_num++;
					}
					
				}
				
				/* Handle log request */
				else if(cmd == CMD_LOG){
					printf("cmd = %d\n", cmd);
					char usr[USERNAME_MAX] = "";
					char other[USERNAME_MAX] = "";
					
					unordered_map<int, string>::iterator iter;
					iter = fd_to_usr.find(client_fd[i]);
					strcpy(usr, iter->second.c_str());

					recv(client_fd[i], other, sizeof(other), 0);
					Historical( usr, other, client_fd[i]);
					puts("LOG OVER");
				}
				
				
				/* Handle build up chat request */
				else if(cmd == CMD_CHAT) {
					int usr_status = 0;
					int found = 0;
					char sender[USERNAME_MAX] = "";
					char receiver[USERNAME_MAX] = "";
					string receiver_str;
					recv(client_fd[i], &receiver, sizeof(receiver), 0);
					
					unordered_map<int, string>::iterator iter;
					unordered_map<string, int>::iterator iter2;
					strcpy(sender, fd_to_usr[client_fd[i]].c_str());
					receiver_str = receiver;
					printf("receiver = %s\n", receiver);
					printf("fd = %d\n", usr_to_fd[receiver]);
					
					iter2 = usr_to_fd.find(receiver_str);
					if(iter2 == usr_to_fd.end() || iter2->second == 0) {
						for(int i = 0; i < usr_num; i++) {
							if(strcmp(usr_list[i], receiver) == 0) {
								usr_status = USER_OFFLINE;
								found = 1;
								break;
							}
						}

						if(!found) 
							usr_status = USER_INVALID;
					}
					
					else 
						usr_status = USER_ONLINE;

					send(client_fd[i], &usr_status, sizeof(usr_status), 0);
					
					if(usr_status == USER_ONLINE) {
						chat[client_fd[i]] = iter2->second;
						chat[iter2->second] = client_fd[i];
					}
					else if(usr_status == USER_OFFLINE){
						fd_to_name[client_fd[i]] = receiver_str;
					}
				}
				
				/* Check friend list */
				else if(cmd == CMD_FRIEND) {
					char filename[USERNAME_MAX*2] = "";
					unordered_map<int, string>::iterator iter;
					iter = fd_to_usr.find(client_fd[i]);
					strcpy(filename, "friend/");
					strcat(filename, iter->second.c_str());
					strcat(filename, ".txt");
					FILE *fp = fopen(filename, "r");
					cout << "filename : " << filename << endl;
					unordered_map<string, int>::iterator iter2;
					char friend_name[USERNAME_MAX] = "";
					while(1) {
						int ret = fscanf(fp, "%s", friend_name);
						usleep(300000);
						send(client_fd[i], &ret, sizeof(ret), 0);
						usleep(300000);
						cout << "ret = " << ret << endl;
						if(ret == EOF)
							break;
						string friendname = friend_name;
						iter2 = usr_to_fd.find(friendname);
						int online = 0;
						if(iter2 == usr_to_fd.end() || iter2->second == 0) {
							online = -1;
							send(client_fd[i], &online, sizeof(online), 0);
						}
						else {
							online = 1;
							send(client_fd[i], &online, sizeof(online), 0);
						}
						cout << "loop\n";
						usleep(300000);
						send(client_fd[i], friend_name, strlen(friend_name), 0);
						cout << friend_name << endl;
						memset(friend_name, '\0', sizeof(friend_name));
					}	
					printf("leave loop\n");
					fclose(fp);
				}

				else if(cmd == CMD_ADDFRIEND || cmd == CMD_UNFRIEND) {
					char friendname[USERNAME_MAX] = "";
					char username[USERNAME_MAX] = "";
					recv(client_fd[i], friendname, sizeof(friendname), 0);
					int invalid = 1;
					for(int i = 0; i < usr_num; i++) {
						if(strcmp(usr_list[i], friendname) == 0) {
							invalid = 0;
							break;
						}
					}
					int ret = 0;
					if(invalid) {
						ret = FRIEND_INVALID;
						send(client_fd[i], &ret, sizeof(ret), 0);
					}
					else {
						unordered_map<int, string>::iterator iter;
						iter = fd_to_usr.find(client_fd[i]);
						strcpy(username, iter->second.c_str());
						int ret = Friend_control(cmd, friendname, username);
						send(client_fd[i], &ret, sizeof(ret), 0);
					}
				}
				else if(cmd == CMD_LOGOUT){
					string usr_tmp = fd_to_usr[client_fd[i]];
					if(chat[client_fd[i]] != 0){
						fd_to_name[chat[client_fd[i]]] = usr_tmp;
						int ret = RECV_PEOPLEOFFLINE;
						send(chat[client_fd[i]], &ret, sizeof(ret), 0);
						chat[chat[client_fd[i]]] = 0;
						chat[client_fd[i]] = 0;
					}
					usr_to_fd.erase(usr_tmp);
					fd_to_usr.erase(client_fd[i]);
					client_fd[i] = 0;	
				}

				/* Send message */
				else if(cmd == CMD_CHATTING || cmd == CMD_OFFLINE) {
					char message[STRING_MAX] = "";
					recv(client_fd[i], &message, sizeof(message), 0);
					char src[USERNAME_MAX] = "";
					unordered_map<int, string>::iterator iter;
					iter = fd_to_usr.find(client_fd[i]);
					strcpy(src, iter->second.c_str());
					char new_message[STRING_MAX+USERNAME_MAX] = "";
					strcat(new_message, src);
					strcat(new_message, ": ");
					strcat(new_message, message);
					
					unordered_map<string, int>::iterator iter2;

					if(cmd == CMD_CHATTING){
						int ret = 1;
						send(chat[client_fd[i]], &ret, sizeof(ret), 0);
						send(chat[client_fd[i]], new_message, strlen(new_message), 0);
						char dest[USERNAME_MAX];
						iter = fd_to_usr.find(chat[client_fd[i]]);
						strcpy(dest, iter->second.c_str());
						Message_file(src, dest, new_message);
					}
					else if(cmd == CMD_OFFLINE){
						char dest[USERNAME_MAX];
						strcpy(dest, fd_to_name[client_fd[i]].c_str());
						Message_file(src, dest, new_message);
						Offline_file(src, dest, new_message);
					}

				}
				
				/* Handle game operation */
				else if(cmd == CMD_GAME) {
					int chess;
					int ret = RECV_GAME;
					recv(client_fd[i], &chess, sizeof(int), 0);
					send(chat[client_fd[i]], &ret, sizeof(ret), 0);
					send(chat[client_fd[i]], &chess, sizeof(chess), 0);
				}

				else if(cmd == CMD_GAME_REQ) {
					int ret = RECV_GAME_REQ;
					send(chat[client_fd[i]], &ret, sizeof(ret), 0);
				}

				else if(cmd == CMD_GAME_RES) {
					int response = 0;
					recv(client_fd[i], &response, sizeof(int), 0);
					send(chat[client_fd[i]], &response, sizeof(response), 0);
				}

				/* Handle file transfer */
				else if(cmd == CMD_FILE) {
					char filename[STRING_MAX] = "";
					recv(client_fd[i], filename, sizeof(filename), 0);
					Transfer_file(client_fd[i], chat[client_fd[i]], filename);
				}

				else if(cmd == CMD_FILE_REQ) {
					char filename[STRING_MAX] = "";
					recv(client_fd[i], filename, sizeof(filename), 0);
					int ret = RECV_FILE;
					send(chat[client_fd[i]], &ret, sizeof(ret), 0);
					send(chat[client_fd[i]], filename, strlen(filename), 0);
				}
				else if(cmd == CMD_FILE_RES) {
					int res = 0;
					recv(client_fd[i], &res, sizeof(int), 0);
					send(chat[client_fd[i]], &res, sizeof(res), 0);
				}
				else if(cmd == CMD_EXIT){
					unordered_map<int, string>::iterator iter;
					iter = fd_to_name.find(client_fd[i]);
					if( iter != fd_to_name.end()){
						fd_to_name.erase(client_fd[i]);
					}
					else{
						unordered_map<string, int>::iterator iter2;
						string usr_tmp;
						int fd_tmp = chat[client_fd[i]];
						chat[client_fd[i]] = 0;
						chat[fd_tmp] = 0;
					}
				}
			}			
		}
	}

	return 0;
}
