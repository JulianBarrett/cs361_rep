#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"


static bool get_args (int, char **);

static bool debug = false;
static uint16_t port = 5000;

static bool get_args(int, char **);

int main(int argc, char **argv) {
    fprintf(stdout, "jknp\n");

    get_args(argc, argv);

    printf("kljfdoksjfl\n");

    if (debug)
        fprintf(stdout, "Starting server on port %d\n", port);

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(get_port()));

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // printf("Server listening on port %d...\n", atoi(get_port()));

    // while (1) {
    //     msg_t received_msg;
    //     int n;

    //     len = sizeof(cliaddr);
    //     n = recvfrom(sockfd, &received_msg, sizeof(received_msg), 0,
    //             (struct sockaddr *)&cliaddr, &len);
    //     if (n < 0) {
    //         perror("recvfrom failed");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    close(sockfd);
    return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv)
{
  int ch = 0;
  while ((ch = getopt (argc, argv, "dh")) != -1)
    {
      switch (ch)
        {
        case 'd':
          debug = true;
          break;
        case 'p':
          port = atoi(optarg);
          break;
        default:
        
          return false;
        }
    }
  return true;
}
