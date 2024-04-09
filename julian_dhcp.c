#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
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


msg_t
parse_packet(uint8_t *packet, int packet_size)
  {
    msg_t msg;
    //     memcpy(&msg, packet, sizeof(msg_t));

    msg.op = packet[0];
    msg.htype = packet[1];
    msg.hlen = packet[2];
    msg.hops = packet[3];
    // 4-byte field
    msg.xid = ntohl(*(uint32_t *)(packet + 4));
    // 2-byte fieldx
    msg.secs = ntohs(*(uint16_t *)(packet + 8));
    // 2-byte field
    msg.flags = ntohs(*(uint16_t *)(packet + 10));
    // 4-byte field
    memcpy(&msg.ciaddr.s_addr, packet + 12, sizeof(uint32_t));
    // a 4-byte field
    memcpy(&msg.yiaddr.s_addr, packet + 16, sizeof(uint32_t));
    // 4-byte field
    memcpy(&msg.siaddr.s_addr, packet + 20, sizeof(uint32_t));
    // 4-byte field
    memcpy(&msg.giaddr.s_addr, packet + 24, sizeof(uint32_t));
    // length is specified by hlen
    memcpy(msg.chaddr, packet + 28, msg.hlen);
    
    // check for MAGICOOKIE TO SIGNAL DHCP
    msg.cookie = ntohl(*(uint32_t *)(packet + 236));
    msg.options = (int)packet[242];
    memcpy(msg.optionsBlock, packet + 243, 50);
    // msg.continued = ntohl(*(uint32_t *)(packet + 243));
    // memcpy(&msg.continued.s_addr, packet + 245, sizeof(uint32_t));
    // msg.leaseTime = ntohs(*(uint16_t *)(packet + 245));
    
    return msg;
}


char *check_constant(uint32_t number) {
    switch (number) {
        case ETH:
            return " [Ethernet (10Mb)]";
            break;
        case IEEE802:
            return " [IEEE 802 Networks]";
            break;
        case ARCNET:
            return " [ARCNET]";
            break;
        case FRAME_RELAY:
            return " [Frame Relay]";
            break;
       case FIBRE:
            return " [Fibre Channel]";
            break;
       case ATM:
            return " [Asynchronous Transmission Mode (ATM)]";
            break;
        default:
            printf("%d does not equal any predefined constant\n", number);
            break;
    }
    return " oops";
}

char *getDHCPMessageType(uint8_t type) {
    //switch (type) {
          switch (type) {

        case DHCPDISCOVER:
            return "DHCP Discover";
        case DHCPOFFER:
            return "DHCP Offer";
        case DHCPREQUEST:
            return "DHCP Request";
        case DHCPDECLINE:
            return "DHCP Decline";
        case DHCPACK:
            return "DHCP ACK";
        case DHCPNAK:
            return "DHCP NAK";
        case DHCPRELEASE:
            return "DHCP Release";
        default:
            return " oops";
    }
}


uint8_t getDHCPLenType(uint8_t type) {
    //switch (type) {
          switch (type) {

        case ETH:
            return ETH_LEN;
        case IEEE802:
            return IEEE802_LEN;
        case ARCNET:
            return ARCNET_LEN;
        case FRAME_RELAY:
            return FRAME_LEN;
        case FIBRE:
            return FIBRE_LEN;
        case ATM:
            return ATM_LEN;
        default:
            return " oops";
    }
}
