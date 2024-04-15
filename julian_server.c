#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>


#include "dhcp.h"
#include "format.h"
#include "port_utils.h"


static bool get_args (int, char **);

static bool debug = false;
static uint16_t port;


static bool get_args(int, char **);

int main(int argc, char **argv) {
  
    port = (uint16_t)strtoul(get_port(), NULL, 10);
    get_args(argc, argv);

    if (debug)
        fprintf(stdout, "Starting server on port %d\n", port);

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    msg_t received_msg;
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
      {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
      }

    memset(&servaddr, 0, sizeof(servaddr));

    // Assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    // Bind socket with server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int n;

        // Receive message from client
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, &received_msg, sizeof(received_msg), 0,
                     (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
          {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
          }

        // Print that i received message
        size_t size = 0;
        fprintf(stdout, "\n++++++++++++++++\nSERVER RECEIVED:\n");
        dump_msg(stdout, &received_msg, size);
        fprintf(stdout, "++++++++++++++++\n");
        break;
    }

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
        // case 'p':
        //   port = atoi(optarg);
        //   break;
        default:
          return false;
        }
    }
  return true;
}
