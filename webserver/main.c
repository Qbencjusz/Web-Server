/* Jakub Pacierpnik 299477 */

#include "header.h"

char recv_buffer[BUFFER_SIZE+1];


int main(int argc, char *argv[]) {
	
	/* declaring variables */
	uint16_t port;
	char default_path[PATH_MAX];
	char method[1024];
	char uri[1024];
	char version[1024];
	char host[1024];
	char connection[1024];
	char filepath[1024];
	char filetype[1024];
	struct stat stat_buf;
		
		
	/* arguments number validation */
	no_args_valid(argc, argv);
	
	
	/* port number validation and initialization */
	port_valid(argv);
	
	if (sscanf(argv[1], "%hu", &port) != 1) {
		ERROR("sscanf error");
	}
	
	
	/* directory validation and initialization */
	dir_valid(argv);
	
	if (sscanf(argv[2], "%s", (char *) default_path) != 1) {
		ERROR("sscanf error");
	}
        
        
        /* creating socket file descriptor */         
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0) {
		ERROR("socket error");
	}
	
	
	/* allowing reuse of local addresses */
	int aaa = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &aaa, sizeof(int));


	/* filling server information */
	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	
	/* binding socket with the server address */
	if (bind (sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
		ERROR("bind error");
	}
	
	
	/* listening for connections on a socket */
	if (listen (sockfd, 64) < 0) {
		ERROR("listen error");
	}
	
	
	/* main loop of our server */
	for (;;) {	
		
		
		/* accepting a connection on a socket */
		int connected_sockfd = accept (sockfd, NULL, NULL);
		if (connected_sockfd < 0) {
			ERROR("accept error")
		}
		
		/* waiting one second for request from client */	
		while (wait_for_request(connected_sockfd) > 0) {
			
			
			/* receiving a message from a socket */ 
			ssize_t bytes_read = recv(connected_sockfd, recv_buffer, BUFFER_SIZE, 0);
			if (bytes_read < 0) {
				ERROR("recv error");
			}
			
			if (bytes_read == 0) {
				break;
			}
		
		
			/* request parsing */
			if(!parse_request(connected_sockfd, recv_buffer, method, uri, version, host, connection)) {
				break;
			}
					
		
			/* request validation */
			if (!request_valid(connected_sockfd, method, uri, version, host)) {
				break;
			}
		
		
			/* creating and checking path */
			char *domain = strtok(host, ":");
				
			strcpy(filepath, default_path);
			
			strcat(filepath, "/");
		
			strcat(filepath, domain);
				
			if(!path_valid(connected_sockfd, filepath)) {
				break;
			}			
		
			if (uri[strlen(uri) - 1] == '/') {
				redirect(connected_sockfd, domain, port, uri, 0);
				break;
			}
				
			strcat(filepath, uri);
			
			if (!path_valid(connected_sockfd, filepath)) {
				break;
			}				   
		
		
			/* getting informations about a file */
			if (stat(filepath, &stat_buf) < 0) {
				client_error(connected_sockfd, "404", "Not Found", "404 Not Found: Server cannot find the requested resource.", "58");
				break;
			}
			
						
			/* checking if file is a directory */
			if (S_ISDIR(stat_buf.st_mode)) {
				redirect(connected_sockfd, domain, port, uri, 1);
				break;
			}
					
				
			/* checking file extension */
			strcpy(filetype, get_file_type(filepath));		
		
				
			/* trying to open a file */
			int fd = open(filepath, O_RDONLY);
			
			if(!open_check(fd, connected_sockfd)) {
				break;
			}
		
		
			/* creating response */
			send_response(filetype, stat_buf, connected_sockfd, fd);
			
				
			if (strcmp(connection, "close") == 0) {
				break;
			}
				
		}
		
		
		/* closing connection */
		if (close (connected_sockfd) < 0) {
			ERROR("close error");
		}
	}
}
