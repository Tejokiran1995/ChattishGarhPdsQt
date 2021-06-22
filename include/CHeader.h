#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <net/if.h>
#include <dirent.h>
#include<math.h>
#include <netdb.h>

#define SUCCESS 0
#define ERROR -1
#define CANCEL 10
