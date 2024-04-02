#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp.h"

void
dump_packet (uint8_t *ptr, size_t size)
{
  size_t index = 0;
  while (index < size)
    {
      fprintf (stderr, " %02" PRIx8, ptr[index++]);
      if (index % 32 == 0)
        fprintf (stderr, "\n");
      else if (index % 16 == 0)
        fprintf (stderr, "  ");
      else if (index % 8 == 0)
        fprintf (stderr, " .");
    }
  if (index % 32 != 0)
    fprintf (stderr, "\n");
  fprintf (stderr, "\n");
}

char *
htype_helper (msg_t *msg)
{
  char *type;
  switch (msg->htype)
  {
    case ETH:
      type = "Ethernet (10Mb)";
      break;
    case IEEE802:
      type = "IEEE 802 Networks";
      break;
    case ARCNET:
      type = "ARCNET";
      break;
    case FRAME_RELAY:
      type = "Frame Relay";
      break;
    case FIBRE:
      type = "Fibre Channel";
      break;
    case ATM:
      type = "Asynchronous Transmission Mode (ATM)";
      break;
    default:
      type = NULL;
      break;
  }
  return type;
}

bool
is_valid_magic_cookie (uint8_t *options)
{
  uint32_t cookie_val = MAGIC_COOKIE;
  uint32_t cookie_comp;
  memcpy (&cookie_comp, options, sizeof (uint32_t));

  return (cookie_comp == cookie_val);
}

void
bootp_message (FILE *output, msg_t *msg)
{
  uint16_t secs_ord = ntohs (msg->secs);

  fprintf (output, "Op Code (op) = %d [%s]\n", msg->op, msg->op == BOOTREQUEST ? "BOOTREQUEST" : "BOOTREPLY");
  fprintf (output, "Hardware Type (htype) = %d [%s]\n", msg->htype, htype_helper (msg));
  fprintf (output, "Hardware Address Length (hlen) = %d\n", msg->hlen);
  fprintf (output, "Hops (hops) = %d\n", msg->hops);
  fprintf (output, "Transaction ID (xid) = %u (0x%02x)\n", ntohl (msg->xid), ntohl (msg->xid));
  fprintf (output, "Seconds (secs) = %d Days, %d:%02d:%02d\n", secs_ord / 86400, (secs_ord % 86400) / 3600, (secs_ord % 3600) / 60, secs_ord % 60);
  fprintf (output, "Flags (flags) = %d\n", msg->flags);
  fprintf (output, "Client IP Address (ciaddr) = %s\n", inet_ntoa (msg->ciaddr));
  fprintf (output, "Your IP Address (yiaddr) = %s\n", inet_ntoa (msg->yiaddr));
  fprintf (output, "Server IP Address (siaddr) = %s\n", inet_ntoa (msg->siaddr));
  fprintf (output, "Relay IP Address (giaddr) = %s\n", inet_ntoa (msg->giaddr));
  fprintf (output, "Client Ethernet Address (chaddr) = ");

  for (int i = 0; i < msg->hlen; i++)
    {
      if (msg->chaddr[i] < 0)
        fprintf (output, "%02x", msg->chaddr[i] & 0xff);
      else
        fprintf (output, "%02x", msg->chaddr[i]);
    }
  fprintf (output, "\n");
}
//Ignore
void
dhcp_parse (FILE *output, uint8_t *options, size_t options_length)
{
  if (is_valid_magic_cookie(options))
      printf("Magic Cookie = [OK]");
  else
    {
      printf("Magic Cookie [Invalid]");
      return;
    }
  size_t index = 0;
  uint8_t op_code = 0;

  while (index < options_length)
    {
      op_code = options[index++];
      switch (op_code)
        {
          case DHCPDISCOVER:
            printf ("Message Type = DHCP Discover");
            break;
          case DHCPOFFER:
            printf ("Message Type = DHCP Offer");
            break;
          case DHCPREQUEST:
            printf ("Message Type = DHCP Request");
            break;
          case DHCPDECLINE:
            printf ("Message Type = DHCP Decline");
            break;
          case DHCPACK:
            printf ("Message Type = DHCP Ack");
            break;
          case DHCPNAK:
            printf ("Message Type = DHCP Nak");
            break;
          case DHCPRELEASE:
            printf ("Message Type = DHCP Release");
            break;
          default:
            Printf ("Unknown DHCP Message Type");
            break;
        }
    }
}
