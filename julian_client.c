#include <stdlib.h>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

uint32_t ip_to_uint32(const char *ip_str);
static bool get_args2 (int argc, char **argv, msg_t *msg);

int
main (int argc, char **argv)
{
  msg_t *msg = malloc(sizeof(msg_t));
  
  if (!get_args2(argc, argv, msg))
    {
      return EXIT_FAILURE;
    }

  size_t size = 0;
  dump_msg(stdout, msg, size);
  free(msg);  // Free allocated memory before returning

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
uint8_t chad[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  for (int i = 0; i < 6; i++)
    {
      msg->chaddr[i] = chad[i];
    }
  msg->options = DHCPDISCOVER;
  // NEED TO FIX HLEN DEPENDING ON THE TYPE OF HARDWARE
  // need to do the dhcp shit within the switch
  for (int i = 1; i < argc; i++)
    {
      switch (argv[i][0]) {
        case '-':
                switch (argv[i][1]) {
                    case 'x':
                        if (i + 1 < argc)
                          {
                            uint32_t xid = atoi(argv[i + 1]);
                            msg->xid = xid;
                          }
                        break;
                    case 't':
                        if (i + 1 < argc)
                          {
                            msg->htype = atoi(argv[++i]); // Convert hardware type to int
                          }
                        break;
                    case 'c':
                        if (i + 1 < argc)
                          {
                            fprintf(stdout, "look:%s\n", argv[i + 1]);
                            const char *input = argv[i + 1];
                            size_t input_length = strlen(input);
                            for (size_t j = 0; j < input_length; j++)
                              {
                                msg->chaddr[j] = (uint8_t)input[j];
                              }
                            msg->chaddr[input_length] = '\0';
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

                            // need to do defaults
                            // add the offset from lengths and codes, should be 2
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
                            
                          }
                        break;
                    default:
                        return false;
                }
                break;
            default:
                // printf("Unknown option: %s\n", argv[i]);
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
