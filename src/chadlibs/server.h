#include <bits/types/FILE.h>
#include <cstdint>
#include <errno.h>
#include <netdb.h> // for getnameinfo()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Usual socket headers
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>

#define SIZE 1024
#define BACKLOG 10 // Passed to listen()

void report(struct sockaddr_in *server_address) {
  char host_buffer[INET6_ADDRSTRLEN];
  char service_buffer[NI_MAXSERV];
  socklen_t addr_len = sizeof(*server_address);
  int err = getnameinfo((const struct sockaddr *)server_address, addr_len,
                        host_buffer, sizeof(host_buffer), service_buffer,
                        sizeof(service_buffer), NI_NUMERICHOST);
  if (err != 0) {
    printf("[CHAD] It's not working!\n");
  }
  printf("\n[CHAD] Server runing at http://%s:%s\n", host_buffer,
         service_buffer);
}

void set_http_header(char http_header[], char html_file[]) {
  FILE *html_data = fopen(html_file, "r");
  char line[1000];
  char response_data[8000];
  while (fgets(line, 1000, html_data)) {
    strcat(response_data, line);
  }
  strcat(http_header, response_data);
}

int server_initializer(uint16_t port) {
  char http_header[8000] = "[OK] HTTP/1.1 200\r\n\n";
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  bind(server_socket, (struct sockaddr *)&server_address,
       sizeof(server_address));
  int listening = listen(server_socket, BACKLOG);
  if (listening < 0) {
    printf("[ERROR] The server is not runnig.");
    return 1;
  }
  char fhtml[20] = "index.html";
  report(&server_address);
  set_http_header(http_header, fhtml);
  int client_socket = 0;
  while (1) {
    client_socket = accept(server_socket, NULL, NULL);
    send(client_socket, http_header, sizeof(http_header), 0);
    close(client_socket);
  }
  return 0;
}
