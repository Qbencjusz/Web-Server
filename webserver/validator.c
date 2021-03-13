/* Jakub Pacierpnik 299477 */

#include "header.h"


/* validating number of arguments */
void no_args_valid(int argc, char *argv[]) {
   	if (argc != 3) {
      		fprintf(stderr, "USAGE: %s PORT DIRECTORY\n", argv[0]);
      		exit(EXIT_FAILURE);
      	}
}


/* validating port number */
void port_valid(char *argv[]) {
	for (size_t i = 0; i < strlen(argv[1]); i++) {
		if (!isdigit(argv[1][i])) {
         		fprintf(stderr, "INVALID PORT NUMBER\n");
         		exit(EXIT_FAILURE);
         	}
	}
    
	if (atoi(argv[1]) > 65535) {
		fprintf(stderr, "INVALID PORT NUMBER\n");
      		exit(EXIT_FAILURE);
      	}  
}


/* checking if given directory exists */
void dir_valid(char *argv[]) {
	char buf[PATH_MAX];
	if (realpath(argv[2], buf) == NULL) {
		fprintf(stderr, "THIS DIRECTORY DOES NOT EXIST\n");
		exit(EXIT_FAILURE);
	}
}


/* validating elements of http request */
int request_valid(int connected_sockfd, char *method, char *uri, char *version, char *host) {
	if (strstr(version, "HTTP/") == NULL) {
		client_error(connected_sockfd, "501", "Not Implemented", "501 Not Implemented: Server does not support the functionality required to fulfill the request.", "96");
		return 0;
	}
		
	if (strcasecmp(method, "GET")) {
		client_error(connected_sockfd, "501", "Not Implemented", "501 Not Implemented: Server does not support the functionality required to fulfill the request.", "96");
		return 0;
	}
				
	if (strstr(uri, "../") != NULL) {
		client_error(connected_sockfd, "403", "Forbidden", "403 Forbidden: Server understood the request but refuses to authorize it.", "74");
		return 0;
	}
		
	if (strstr(host, "localhost") == NULL && strstr(host, "lab108-18") == NULL) {
		client_error(connected_sockfd, "404", "Not Found", "404 Not Found: Server cannot find the requested resource.", "58");
		return 0;
	}
	
	return 1;
}


/* checking if path exists */
int path_valid(int connected_sockfd, char *filepath) {
	char buf[PATH_MAX];
	
	if (realpath(filepath, buf) == NULL) {
		client_error(connected_sockfd, "404", "Not Found", "404 Not Found: Server cannot find the requested resource.", "58");
		return 0;
	}
	
	return 1;
}


/* checking if file opened correctly */
int open_check(int fd, int connected_sockfd) {
	if (fd < 0 && errno == EACCES) {
		client_error(connected_sockfd, "403", "Forbidden", "403 Forbidden: Server understood the request but refuses to authorize it.", "74");
		return 0;
	}
		
	if (fd < 0) {
		client_error(connected_sockfd, "404", "Not Found", "404 Not Found: Server cannot find the requested resource.", "58");
		return 0;
	}
	
	return 1;
}	
