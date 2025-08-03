#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_LEN 128

void fatal(char* message) {
	// Print error message and exit
	perror(message);
	exit(EXIT_FAILURE);
}

void socket_comm() {
	const char* SOCKET_PATH = "/home/tux/temp/server_sock";
	int socket_fd;

	// Create socket
	socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		fatal("Error while initializing socket");
	}

	// Bind socket to SOCKET_PATH
	socklen_t socket_addr_size;
	struct sockaddr_un socket_addr;

	memset(&socket_addr, 0, sizeof(struct sockaddr_un)); // Initialize to 0

	socket_addr.sun_family = AF_UNIX;
	strncpy(socket_addr.sun_path, SOCKET_PATH, sizeof(socket_addr.sun_path) - 1);
	socket_addr_size = sizeof(socket_addr);

	if (bind(socket_fd, (struct sockaddr *) &socket_addr, sizeof(struct sockaddr_un)) == -1) {
		fatal("Error while binding socket");
	} else {
		printf("Successfully generated server-side socket\n");
	}

	// Mark as passive
	if (listen(socket_fd, 8) == -1) {
		fatal("Cannot mark socket as passive");
	} 
	
	// Connect to client and receive data
	ssize_t numread;
	char buffer[BUFFER_LEN + 1];
  	while (1) {
		// Connect to client
  		int connect_fd = accept(socket_fd, (struct sockaddr*) &socket_addr, &socket_addr_size);
  		if (connect_fd == -1) {
  			fatal("Cannot accept connections");
			sleep(1);
  		}
		
		printf("Connected to client\n");

		// Receive data
		while ((numread = read(connect_fd, buffer, BUFFER_LEN)) > 0) {
			buffer[numread] = '\0';
			printf("%s", buffer); 
		}
	}
}

int main() {
	socket_comm();
	return 0;
}

