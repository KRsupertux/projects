#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

void fatal(char* message) { 
	// Print message and exit
	perror(message);
	exit(EXIT_FAILURE);
}

long get_data(int argc, char* argv[], char** buffer) {
	// Command line argument must be path to file
	// option "-h" or "--help" prints help
	if (argc != 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
		printf("Usage: %s filepath\n",argv[0]);
		printf("Sends data to the server\n");
		exit(EXIT_SUCCESS);
	}

	// Open file
	FILE* fp;
	fp = fopen(argv[1],"r");
	if (fp == NULL) {
		fatal("Cannot open file");
	}

	// Determine file size
	if (fseek(fp, 0, SEEK_END) == -1) {
		fatal("Error determining file size");
	}

	long size = ftell(fp);
	if (size == -1) {
		fatal("Error determining file size");
	}
	rewind(fp);
	
	// Allocate memory for buffer
	*buffer = malloc(size + 1);
	if (*buffer == NULL) {
		fatal("Error allocating memory");
	}

	memset(*buffer, 0, size + 1);

	// Read file and store data in buffer
	fread(*buffer, 1, size, fp);
	if (ferror(fp) != 0) {
		fatal("Error reading file");
	}

	fclose(fp);

	return size;
}

void socket_comm(char* buffer, long size) {
	const char* PEER_SOCKET_PATH = "/home/tux/temp/server_sock";
	int socket_fd;

	// Create socket
	socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		fatal(strerror(errno));
	}
	
	// Connect to server
	struct sockaddr_un peer_socket_addr;
	memset(&peer_socket_addr, 0, sizeof(struct sockaddr_un));
	peer_socket_addr.sun_family = AF_UNIX;
	strncpy(peer_socket_addr.sun_path, PEER_SOCKET_PATH, sizeof(peer_socket_addr.sun_path) - 1);
	
	if (connect(socket_fd, (struct sockaddr*) &peer_socket_addr, sizeof(peer_socket_addr)) == -1) {
		fatal("Error while connecting to server");
	}
	
	printf("Connected to server.\n");

	// Send data to server
	if (write(socket_fd, buffer, size) == -1) {
		fatal("Error sending data to server");
	}
	return;
}

int main(int argc, char* argv[]) {

	char* file_buffer;
	long file_size;
	
	// Get data and file size
	file_size = get_data(argc, argv, &file_buffer);

	// Send data to server
	socket_comm(file_buffer, file_size);

	// Free memory
	free(file_buffer);
	return 0;
}


