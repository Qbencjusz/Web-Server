/* Jakub Pacierpnik 299477 */

#include "header.h"


/* function for sending 403, 404, 501 http responses */
void client_error(int connected_sockfd, char *errnum, char *errname, char *msg, char *msg_len) {
	char reply_msg[1024];
	if (sprintf(reply_msg, "HTTP/1.1 %s %s\r\nContent-Type: text/plain\r\nContent-Length: %s\r\n\r\n%s\n", errnum, errname, msg_len, msg) < 0) {
		ERROR("sprintf error");
	}
	size_t reply_msg_len = strlen(reply_msg);
	ssize_t bytes_sent = send (connected_sockfd, reply_msg, reply_msg_len, MSG_NOSIGNAL);
	if (bytes_sent < 0 && errno != EPIPE) {
		ERROR("send1 error");
	}
}


/* function for sending 301 http response */
void moved_perm_error(int connected_sockfd, char *errnum, char *errname, char *location) {
	char reply_msg[1024];
	if (sprintf(reply_msg, "HTTP/1.1 %s %s\r\nLocation: http://%s\r\n\r\n", errnum, errname, location) < 0) {
		ERROR("sprintf error");
	}
	size_t reply_msg_len = strlen(reply_msg);
	ssize_t bytes_sent = send (connected_sockfd, reply_msg, reply_msg_len, MSG_NOSIGNAL);
	if (bytes_sent < 0 && errno != EPIPE) {
		ERROR("send2 error");
	}
}


/* function calling moved_perm_error and indicating which type of redirection occures */
void redirect(int connected_sockfd, char *domain, uint16_t port, char *uri, int is_dir) {
	char *location;
	if (is_dir) {
		asprintf(&location, "%s:%hu%s/index.html", domain, port, uri);
		moved_perm_error(connected_sockfd, "301", "Moved Permanently", location);
		free(location);
	}
	else {
		asprintf(&location, "%s:%hu%sindex.html", domain, port, uri);
		moved_perm_error(connected_sockfd, "301", "Moved Permanently", location);
		free(location);
	}
}


/* determinating content type of http response based on file extension */
const char* get_file_type(char *filepath) {
	if (strstr(filepath, ".txt")) {
		return "text/plain";
	}
	else if (strstr(filepath, ".html")) {
		return "text/html";
	}
	else if (strstr(filepath, ".css")) {
		return "text/css";
	}
	else if (strstr(filepath, ".jpg") || strstr(filepath, ".jpeg")) {
		return "image/jpeg";
	}
	else if (strstr(filepath, ".png")) {
		return "image/png";
	}
	else if (strstr(filepath, ".pdf")) {
		return "application/pdf";
	}
	else {
		return "application/octet-stream";
	}
}


/* function for sending 200 http response with file content */
void send_response(char *filetype, struct stat stat_buf, int connected_sockfd, int fd) {
	char reply_msg[2048];
	if (sprintf(reply_msg, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", filetype, (int)stat_buf.st_size) < 0) {
		ERROR("sprintf error")
	}
	size_t reply_msg_len = strlen(reply_msg);
	ssize_t bytes_sent = send (connected_sockfd, reply_msg, reply_msg_len, MSG_NOSIGNAL);
	if (bytes_sent < 0 && errno != EPIPE) {
		ERROR("send3 error");
	}
		
	char *ptr = mmap(0, stat_buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	ssize_t bytes_sent2 = send(connected_sockfd, ptr, stat_buf.st_size, MSG_NOSIGNAL);
	if (bytes_sent2 < 0 && errno != EPIPE) {
		ERROR("send4 error");
	}
	munmap(ptr, stat_buf.st_size);
	
	if (close(fd) < 0) {
		ERROR("close error")
	}
}
