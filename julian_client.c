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

uint32_t ip_to_uint32(const char *ip_str);
static bool get_args2 (int argc, char **argv, msg_t *msg);

int
main(int argc, char **argv) {
    
    msg_t *msg = malloc(sizeof(msg_t));
    get_args2(argc, argv, msg);
    size_t size = 0;
    dump_msg(stdout, msg, size);

    if (argc > 1 && strcmp(argv[1], "-p") == 0)
      {
        int sockfd;
      struct sockaddr_in servaddr;

      sockfd = socket(AF_INET, SOCK_DGRAM, 0);
      if (sockfd < 0)
        {
          perror("socket!!!");
          return EXIT_FAILURE;
        }

      memset(&servaddr, 0, sizeof(servaddr));

      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons(atoi(get_port()));
      servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

      if (sendto(sockfd, msg, sizeof(msg_t), 0,
               (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
          perror("failed");
          close(sockfd);
          free(msg);
          return EXIT_FAILURE;
        }

      close(sockfd);
      }
      free(msg);
      return EXIT_SUCCESS;
}

static bool
get_args2 (int argc, char **argv, msg_t *msg)
{
  int optionsCount = 0;
  msg->op = 1;
  msg->xid = 42;
  msg->htype = ETH;
  msg->hlen = 6;

  const char *input = "010203040506";
  size_t input_length = strlen(input);
  if (input_length % 2 == 0)
    {
      for (size_t j = 0; j < input_length; j += 2)
        {
          char byte_str[3] = {input[j], input[j + 1], '\0'};
          uint8_t byte = (uint8_t)strtoul(byte_str, NULL, 16);
          msg->chaddr[j / 2] = byte;
        }
    }

  msg->options = DHCPDISCOVER;
  msg->cookie = MAGIC_COOKIE;

  size_t ip_length = strlen("127.0.0.1");
  msg->optionsBlock[optionsCount++] = 54;
  msg->optionsBlock[optionsCount++] = ip_length;
  char ip_address[] = "127.0.0.1";
  uint32_t ip_value = ip_to_uint32(ip_address);
  memcpy(&msg->optionsBlock[optionsCount], &ip_value, sizeof(uint32_t));
  optionsCount += sizeof(uint32_t);

  size_t ip_length2 = strlen("127.0.0.2");
  msg->optionsBlock[optionsCount++] = 50;
  msg->optionsBlock[optionsCount++] = ip_length2;
  char ip_address2[] = "127.0.0.2";
  uint32_t ip_value2 = ip_to_uint32(ip_address2);
  memcpy(&msg->optionsBlock[optionsCount], &ip_value2, sizeof(uint32_t));
  optionsCount += sizeof(uint32_t);

  optionsCount = 0;

  for (int i = 1; i < argc; i++)
    {
      switch (argv[i][0]) {
        case '-':
                switch (argv[i][1]) {
                    case 'x':
                        if (i + 1 < argc)
                          {
                            uint32_t xid = atoi(argv[++i]);
                            msg->xid = xid;
                            
                          }
                        break;
                    case 't':
                        if (i + 1 < argc)
                          {
                            msg->htype = atoi(argv[++i]);
                            msg->hlen = getDHCPLenType(msg->htype);
                          }
                        break;
                    case 'c':
                        if (i + 1 < argc)
                          {
                            const char *input = argv[++i];
                            size_t input_length = strlen(input);
                            if (input_length % 2 != 0)
                              {
                                return false;
                              }
                            for (size_t j = 0; j < input_length; j += 2)
                              {
                                char byte_str[3] = {input[j], input[j + 1], '\0'};
                                uint8_t byte = (uint8_t)strtoul(byte_str, NULL, 16);
                                msg->chaddr[j / 2] = byte;
                              }
                          }
                        break;
                    case 'm':
                        if (i + 1 < argc)
                          {
                            msg->cookie = MAGIC_COOKIE;
                            msg->options = atoi(argv[++i]);
                          }
                        break;
                    case 's':
                        if (i + 1 < argc)
                          {
                            
                            msg->cookie = MAGIC_COOKIE;
                            size_t ip_length = strlen(argv[i + 1]);
                            msg->optionsBlock[optionsCount++] = 54;
                            msg->optionsBlock[optionsCount++] = ip_length;
                            uint32_t ip_value = ip_to_uint32(argv[++i]);
                            memcpy(&msg->optionsBlock[optionsCount], &ip_value, sizeof(uint32_t));
                            optionsCount += sizeof(uint32_t);
                          }
                        break;
                    case 'r':
                        if (i + 1 < argc)
                          {
                            optionsCount = 6;
                            msg->cookie = MAGIC_COOKIE;
                            size_t ip_length = strlen(argv[i + 1]);
                            msg->optionsBlock[optionsCount++] = 50;
                            msg->optionsBlock[optionsCount++] = ip_length;
                            uint32_t ip_value = ip_to_uint32(argv[++i]);
                            memcpy(&msg->optionsBlock[optionsCount], &ip_value, sizeof(uint32_t));
                            optionsCount += sizeof(uint32_t);
                          }
                        break;
                    case 'p':
                        if (i + 1 < argc)
                          {
                            msg->cookie = MAGIC_COOKIE;
                            msg->xid = 0;
                          }                  
                        break;
                    default:
                        return false;
                }
                break;
            default:
                return false;
        }
    }

  return true;
}

static bool
get_args (int argc, char **argv)
{
  return true;
}

uint32_t ip_to_uint32(const char *ip_str) {
    char *endptr;
    char *token;
    uint32_t ip = 0;

    for (int i = 0; i < 4; i++) {
        token = strtok((i == 0) ? (char *)ip_str : NULL, ".");
        long int octet = strtol(token, &endptr, 10);
        ip |= (uint32_t)(octet << (i * 8));
    }

    return ip;
}
