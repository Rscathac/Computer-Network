#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>

#define CLIENT_MAX 1024
#define BUFFER_MAX 1024


int main(int argc, char *argv[]) {
	
	int host = atoi(argv[1]);
	int socket_fd, max_fd, new_sd;
	int client_fd[CLIENT_MAX] = {0};
	struct sockaddr_in server_addr, client_addr;
	char welcome[BUFFER_MAX] = "Kobe Bryant";
	char goodbye[BUFFER_MAX] = "Roger";
	char buffer[BUFFER_MAX] = "";
	int packet = 0;
	fd_set readfds;
	
	
	// Create a socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		puts("Fail to open socket\n");
		return -1;
	}
	
	// Set up server address
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(host);

	int addr_len = sizeof(client_addr);
	
	// Bind address
	if(bind(socket_fd, (struct sockaddr *)&server_addr, addr_len) < 0) {
		puts("Bind failed");
		return -1;
	}

	// Listen 
	listen(socket_fd, 5);

	while(1) {
		
		// fd set set up
		FD_ZERO(&readfds);
		FD_SET(socket_fd, &readfds);
		max_fd = socket_fd;

		for(int i = 0; i < CLIENT_MAX; i++) {
			if(client_fd[i] > 0) 
				FD_SET(client_fd[i], &readfds);
			if(client_fd[i] >= max_fd)
				max_fd = client_fd[i];
		}
		
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		if(select(max_fd+1, &readfds, NULL, NULL, &tv) < 0) {
			continue;
		}

		// Incoming connection
		if(FD_ISSET(socket_fd, &readfds)) {
			new_sd = accept(socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
			if(new_sd < 0) {
				puts("Accept failed");
				return -1;
			}

			printf("new connection [%s:%d]\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
	
			// Send welcome message
			if(send(new_sd, welcome, strlen(welcome), 0) != strlen(welcome)) {
				puts("Send welcome failed");
				return -1;
			}

			// Add new socket to the client_fd set
			for(int i = 0; i < CLIENT_MAX; i++) {
				if(client_fd[i] == 0) {
					client_fd[i] = new_sd;
					break;
				}
			}

		}
		// Some client operation
		for(int i = 0; i < CLIENT_MAX; i++) {
			if(client_fd[i] > 0 && FD_ISSET(client_fd[i], &readfds)) {
				
				memset(buffer, '\0', sizeof(buffer));
				int ret = recv(client_fd[i], buffer, BUFFER_MAX, 0);
				getpeername(client_fd[i], (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
				// Disconnect
				if(ret <= 0) {
					printf("lost connection from [%s:%d]\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
					close(client_fd[i]);
					client_fd[i] = 0;
				}
				
				// receive message succeed
				else {

					printf("recv from [%s:%d]\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
//					printf("Message : %s\n", buffer);
					if(send(client_fd[i], goodbye, strlen(goodbye), 0) < strlen(goodbye)) 
						puts("Return message to client failed");
				}
			}
		}

	}
	
	

	return 0;
}
