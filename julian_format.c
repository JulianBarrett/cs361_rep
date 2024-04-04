#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>


#include "dhcp.h"
#include "format.h"


void
dump_msg (FILE *output, msg_t *msg, size_t size)
{

  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "BOOTP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  // TODO: Print out the BOOTP fields as specified in the assignment

  fprintf(output, "Op Code (op) = %d [%s]\n", msg->op, (msg->op == BOOTREQUEST) ? "BOOTREQUEST" : "BOOTREPLY");
  fprintf(output, "Hardware Type (htype) = %d%s\n", msg->htype, check_constant(msg->htype));
  fprintf(output, "Hardware Address Length (hlen) = %d\n", msg->hlen);
  fprintf(output, "Hops (hops) = %d\n", msg->hops);
  fprintf(output, "Transaction ID (xid) = %u (0x%x)\n", msg->xid, msg->xid);
  fprintf(output, "Seconds (secs) = %u Days, %u:%02u:%02u\n", msg->secs / 86400, (msg->secs % 86400) / 3600, (msg->secs % 3600) / 60, msg->secs % 60);

  fprintf(output, "Flags (flags) = %u\n", msg->flags);
  fprintf(output, "Client IP Address (ciaddr) = %s\n", inet_ntoa(msg->ciaddr));
  fprintf(output, "Your IP Address (yiaddr) = %s\n", inet_ntoa(msg->yiaddr));
  fprintf(output, "Server IP Address (siaddr) = %s\n", inet_ntoa(msg->siaddr));
  fprintf(output, "Relay IP Address (giaddr) = %s\n", inet_ntoa(msg->giaddr));

  // fprintf(output, "%x\n", msg->cookie);

  // if dhcp then print first
    if (msg->cookie == MAGIC_COOKIE)
    {
      fprintf(output, "Client Ethernet Address (chaddr) = ");
      for (int i = 0; i < msg->hlen; i++) {
        if (msg->chaddr[i] != 00) {
          fprintf(output, "%02x", msg->chaddr[i]);
        } else if (msg->chaddr[i + 1] != 00) {
          fprintf(output, "%02x", msg->chaddr[i]);
        }
      }
      fprintf(output, "\n");
    }

  if (msg->cookie == MAGIC_COOKIE)
    {
      fprintf (output, "------------------------------------------------------\n");
      fprintf (output, "DHCP Options\n");
      fprintf (output, "------------------------------------------------------\n");
    }

  // if not dhcp then print aftewards
  if (msg->cookie != MAGIC_COOKIE)
    {
      fprintf(output, "Client Ethernet Address (chaddr) = ");
      for (int i = 0; i < msg->hlen; i++) {
        if (msg->chaddr[i] != 00) {
          fprintf(output, "%02x", msg->chaddr[i]);
        } else if (msg->chaddr[i + 1] != 00) {
          fprintf(output, "%02x", msg->chaddr[i]);
        }
      }
      fprintf(output, "\n");
    }

  // TODO: Print out the DHCP fields as specified in the assignment

    if (msg->cookie == MAGIC_COOKIE)
    {
      fprintf(output, "Magic Cookie = [OK]\n");
      fprintf(output, "Message Type = %s\n", getDHCPMessageType(msg->options));

      if (msg->options == DHCPOFFER || msg->options == DHCPACK) 
        {
          uint32_t secs = ntohl(*(uint32_t *)(msg->optionsBlock + 2)); // + 1 originally
          fprintf(output, "IP Address Lease Time = %u Days, %u:%02u:%02u\n", secs / 86400, (secs % 86400) / 3600, (secs % 3600) / 60, secs % 60);
          fprintf(output, "Server Identifier = ");

          for (int i = 8; i < 11; i++)
            {
              fprintf(output, "%d.", msg->optionsBlock[i]);
            }
          fprintf(output, "%d", msg->optionsBlock[11]);
          fprintf(output, "\n");
        }

      if (msg->options == DHCPRELEASE || msg->options == DHCPNAK)
        {
          fprintf(output, "Server Identifier = ");
            for (int i = 2; i < 5; i++)
              {
                fprintf(output, "%d.", msg->optionsBlock[i]);
              }
            fprintf(output, "%d\n", msg->optionsBlock[5]);
      }

      if (msg->options == DHCPDECLINE || msg->options == DHCPREQUEST)
        {

          fprintf(output, "Request = ");
          if (msg->optionsBlock[0] == 50)
            {
              for (int i = 2; i < 5; i++)
                {
                  fprintf(output, "%d.", msg->optionsBlock[i]);
                }
              fprintf(output, "%d\n", msg->optionsBlock[5]);
            }
          else
            {
              for (int i = 8; i < 11; i++)
                {
                  fprintf(output, "%d.", msg->optionsBlock[i]);
                }
              fprintf(output, "%d\n", msg->optionsBlock[11]);
            }

          fprintf(output, "Server Identifier = ");
          if (msg->optionsBlock[0] == 54)
            {
              for (int i = 2; i < 5; i++)
                {
                  fprintf(output, "%d.", msg->optionsBlock[i]);
                }
              fprintf(output, "%d\n", msg->optionsBlock[5]);
            }
          else
            {
              for (int i = 8; i < 11; i++)
                {
                  fprintf(output, "%d.", msg->optionsBlock[i]);
                }
              fprintf(output, "%d\n", msg->optionsBlock[11]);
            }
        }


    }

}

