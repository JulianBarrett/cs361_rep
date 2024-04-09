#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dhcp.h"
#include "format.h"

int
main (int argc, char **argv)
{
  // TODO: Open the file specified on the command line
  // Note: The file contains a binary network packet

  const char *file_name = argv[1];
  int file_descriptor = open(file_name, O_RDONLY);
  if (file_descriptor == -1)
    {
      perror("didint open the file");
      return EXIT_FAILURE;
    }

  // TODO: Determine the size of the packet in bytes
  // Note: You have used the fstat() function before
    struct stat file_info;
    fstat(file_descriptor, &file_info);
    size_t packet_size = file_info.st_size;

  // TODO: Define a uint8_t * variable
    uint8_t *packet;


  // TODO: Allocate enough space to hold the packet (store in your varaible)
    packet = (uint8_t *)malloc (packet_size);

  // TODO: Read the packet data from the file into your variable
  read(file_descriptor, packet, packet_size);


  // TODO: Call dump_msg() from format.c to print the packet data to stdout
  msg_t msg = parse_packet(packet, packet_size);
  dump_msg(stdout, &msg, packet_size);

  // TODO: Don't forget to free the dynamically-allocated space
  free(packet);
  close(file_descriptor);

  return EXIT_SUCCESS;
}
