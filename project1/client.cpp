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

#define BUFFER_MAX 1024
#define HOST_MAX 1024

int packet_num = -1;
double to = 1000;
char host[HOST_MAX][128];
int port[HOST_MAX];

int hostname_to_ip(char *hostname, char *ip) {
	struct hostent *he;
	struct in_addr **addr_list;

	if((he = gethostbyname(hostname)) == NULL) {
			puts("get hostname failed");
			return 0;
	}

	addr_list = (struct in_addr **)he->h_addr_list;

	for(int i = 0; addr_list[i] != NULL; i++) {
		strcpy(ip, inet_ntoa(*addr_list[i]));
		return 0;
	}
	return 0;
	
}

void *socket_thread(void *thread_arg) {
	int host_idx = *(int *)thread_arg;

	int sec = to / 1000;
	int usec = fmod(to,1000)*1000;

	int socket_fd;
	struct sockaddr_in client_addr;	
	clock_t t;
	int packet = packet_num;

	// Address setup
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(host[host_idx]);
	client_addr.sin_port = htons(port[host_idx]);

	// Open socket
	socket_fd = socket(AF_INET ,SOCK_STREAM, 0);

	if(socket_fd < 0) {
		puts("Open socket failed");
		pthread_exit(NULL);

	}

	// Connect
	if(connect(socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
		printf("timeout when connect to [%s]\n", host[host_idx]);
		pthread_exit(NULL);
	}

	char tmp[BUFFER_MAX] = "";
	recv(socket_fd, tmp, BUFFER_MAX, 0);

	while(packet != 0) {

		t = clock();
		char greet[BUFFER_MAX] = "Hi, I'm going to send you some message";
		int ret = send(socket_fd, greet, strlen(greet), 0);
		if(ret < 0) {
			puts("Send message failed");
			pthread_exit(NULL);
		}

		struct timeval timeout;
		timeout.tv_sec = sec;
		timeout.tv_usec = usec;
	
		int sret = 0;
		sret = setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&timeout, sizeof(timeout));
		
		if(sret < 0){
			puts("setsockopt failed");	
			pthread_exit(NULL);
		}
	
//		t = clock();

		char buffer[BUFFER_MAX] = "";
		int rec_ret = recv(socket_fd, buffer, BUFFER_MAX, 0);
		if(rec_ret < 0){
			printf("timeout when connect to [%s]\n", host[host_idx]);
			if(rec_ret == ECONNRESET)  {
				puts("lose connection");
				pthread_exit(NULL);
			}
		}

		t = clock() - t;
		double rtt = ((double)t)/CLOCKS_PER_SEC;
		if((rtt*1000) >= to) 
			printf("timeout when connect to [%s]\n", host[host_idx]);
	
		else 
			printf("recv from [%s], RTT = %f\n", host[host_idx], rtt*1000);
		packet--;
	}

	close(socket_fd);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

	if(argc < 2) {
		puts("Error command");
		return 0;
	}

	char check_n[8] = "-n";
	char check_t[8] = "-t";
	int host_num = 0;
	char host_name[HOST_MAX][128];

	memset(host, '\0', sizeof(host));
	memset(host_name, '\0', sizeof(host_name));

	
	// Read command line arguments
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], check_n) == 0) {
			i++;
			packet_num = atoi(argv[i]);
			if(packet_num == 0)
				packet_num = -1;
		}

		else if(strcmp(argv[i], check_t) == 0) {
			i++;
			to = atof(argv[i]);
		}

		else {
			int cut = 0;
			for(int j = 0; j < strlen(argv[i]); j++) {
				if(argv[i][j] == ':') {
					cut = j+1;
					strncpy(host_name[host_num], argv[i], j);
					hostname_to_ip(host_name[host_num], host[host_num]);
					break;
				}
			}
			
			char tmp_port[32] = "";
			for(int j = 0; cut < strlen(argv[i]); cut++, j++) 
				tmp_port[j] = argv[i][cut];

			port[host_num] = atoi(tmp_port);
			host_num++;
		}
	}
	
//	printf("pnum = %d, timeout = %f, [%s:%d]\n", packet_num, to, host[1], port[1]);
	
	pthread_t host_thread[host_num];
	int host_idx[host_num];
	for(int i = 0; i < host_num; i++) 
		host_idx[i] = i;
	
	for(int i = 0; i < host_num; i++) 
		pthread_create(&host_thread[i], NULL, socket_thread, &host_idx[i]);
		
	for(int i = 0; i < host_num; i++) 
		pthread_join(host_thread[i], NULL);
	
	return 0;
}
