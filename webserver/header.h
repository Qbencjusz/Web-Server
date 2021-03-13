/* all required includes */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <ctype.h>


/* defines */
#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(EXIT_FAILURE); }
#define BUFFER_SIZE 10000000	// 10 MB


/* function prototypes */
void client_error(int connected_sockfd, char *errnum, char *errname, char *msg, char *msg_len);
void moved_perm_error(int connected_sockfd, char *errnum, char *errname, char *location);
void no_args_valid(int argc, char *argv[]);
void port_valid(char *argv[]);
void dir_valid(char *argv[]);
int wait_for_request(int connected_sockfd);
int parse_request(int connected_sockfd, char *recv_buffer, char *method, char *uri, char *version, char *host, char *connection);
int request_valid(int connected_sockfd, char *method, char *uri, char *version, char *host);
int path_valid(int connected_sockfd, char *filepath);
void redirect(int connected_sockfd, char *domain, uint16_t port, char *uri, int is_dir);
void send_response(char *filetype, struct stat stat_buf, int connected_sockfd, int fd);
const char* get_file_type(char *filepath);
int open_check(int fd, int connected_sockfd);
