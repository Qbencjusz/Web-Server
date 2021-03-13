/* Jakub Pacierpnik 299477 */

#include "header.h"


/* waiting a certain number of seconds for a client request */
int wait_for_request(int connected_sockfd) {

	fd_set descriptors;
	FD_ZERO (&descriptors);
	FD_SET (connected_sockfd, &descriptors);
   
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
   
	int ready = select (connected_sockfd+1, &descriptors, NULL, NULL, &tv);
	if (ready < 0) {
		fprintf(stderr, "SELECT ERROR\n");
		return EXIT_FAILURE;
   	}
   
   	return ready;
}


/* parsing request */
int parse_request(int connected_sockfd, char *recv_buffer, char *method, char *uri, char *version, char *host, char *connection) {
	char *split_req;
	
	split_req = strtok(recv_buffer, "\r\n");
		
	if (sscanf(split_req, "%s %s %s", method, uri, version) != 3) {
		client_error(connected_sockfd, "501", "Not Implemented", "501 Not Implemented: Server does not support the functionality required to fulfill the request.", "96");
		return 0;
	}
						
	while (split_req != NULL) {
		split_req = strtok(NULL, "\r\n");
		if (split_req != NULL) {
			if (split_req == strstr(split_req, "Host: ")) {
				if (sscanf(split_req+6, "%s\n", host) != 1) {
					ERROR("sscanf error");
				}
			}
			if (split_req == strstr(split_req, "Connection: ")) {
				if (sscanf(split_req+12, "%s\n", connection) != 1) {
					ERROR("sscanf error");
				}
			}
		}
	}
	
	return 1;
}

