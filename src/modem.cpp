#include "include/modem.h"
#include "include/misc.h"

#define SERVER_PORT 80
#define BUF_SIZE    256


modem::modem()
{
}


int modem::Modem_Open(char *weight, char *comm_port)
{

    CMisc miscObj;
    char BUFF[1024];
    memset(BUFF, 0, sizeof(BUFF));
    qDebug()<<"Getting Wt From WM@@@@@@@@@@@@";

    int ret = GetWtFromWeingMachine(BUFF, comm_port );
    if(ret<0)
    {
        miscObj.DisplayWideMsgBox((char *)QString::number(ret).toAscii().data());

        return -1;
    }

    miscObj.DisplayWideMsgBox((char *)BUFF);

    ret = Parse(BUFF, weight);
    if(ret == -2)
        return -2;


    if(strlen(BUFF) <= 0)
    {
        QMessageBox msg;
        msg.setText("No Response");
        msg.exec();

    }

    return 0;
}

int modem::GetWtFromWeingMachine(char *recv, char *comm_port )
{
    struct termios my_termios;
    int Baudrate=0, l;
    int count=6;
    Baudrate = B9600;
    static int fd;

    memset(&my_termios, 0, sizeof(struct termios));

    fd = ::open("/dev/ttymxc0", O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
    if(fd < 0)
    {
        fprintf(stderr,"Unable to open port \n Please check the Arguments\n");
        CMisc miscObj;
        miscObj.DisplayWideMsgBox((char *)"Unable to open Weighing Machine port");
        return -1;
    }

    tcflush(fd,TCIFLUSH);
    my_termios.c_cflag=Baudrate|CS8|CREAD|CLOCAL;//|PARENB;
    my_termios.c_cc[VMIN]=0;
    my_termios.c_cc[VTIME]=0;
    tcsetattr(fd,TCSANOW,&my_termios);

    usleep(300000);

    while(count--)
    {
        memset(recv,0x00,1024);
        l = ::read(fd,recv,12);
        if(l > 35  )
        {
            fprintf(stdout,"^^^^^^^^%s^^^^^^^^^^^^^%d\n",recv ,count );
            break;
        }

        usleep(400000);
    }

    ::close(fd);
    return 0;
}


int modem::Parse(char *Buff, char *weight)
{
    int len=0;
    char data[20]="";
    char *p, *q;

    char tmp1[32], tmp2[32];
    memset(tmp1, 0, sizeof(tmp1));
    memset(tmp2, 0, sizeof(tmp2));
    qDebug()<<"Input to Parse : "<<Buff<<"#######";

    len = strlen(Buff);
    printf("length===============%d\n",len);

    p = strchr(Buff, '+');
    if(p==NULL)
    {
        return -2;
        p = strchr(Buff, '-');
        if(p==NULL)
        {
            qDebug()<<"wt capturing failed";
            return -1;
        }
    }
    q = p+1;
    if(q == NULL)
    {
        qDebug()<<"unable to parse 1";
        return -1;
    }
    p = strstr(q, " Kg");
    if(p == NULL)
    {
        qDebug()<<"unable to parse 2";
        return -1;
    }
    strncpy(data, q, p-q);
    qDebug()<<"Weight = "<<data;
    strcpy(weight, data);
    weight[strlen(data)] = '\0';

    qDebug()<<"Weight Buff = "<<weight;

    return 0;
}


void modem::setmodemparams(int baudrate,int fd)
{
        struct termios newtio;
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = CS8 | CREAD;

        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        newtio.c_lflag = 0;
        newtio.c_cc[VTIME]    = 0;
        newtio.c_cc[VMIN]     = 0;
        switch (baudrate)
        {
                case 300:    baudrate=B300; break;
                case 1200:   baudrate=B1200; break;
                case 2400:   baudrate=B2400; break;
                case 9600:   baudrate=B9600; break;
                case 19200:  baudrate=B19200; break;
                case 38400:  baudrate=B38400; break;
#ifdef B57600
                case 57600:  baudrate=B57600; break;
#endif
#ifdef B115200
                case 115200: baudrate=B115200; break;
#endif
#ifdef B230400
                case 230400: baudrate=B230400; break;
#endif
                default: baudrate=B9600;
        }
        cfsetispeed(&newtio,baudrate);
        cfsetospeed(&newtio,baudrate);
        tcsetattr(fd,TCSANOW,&newtio);
}


int modem::sockComm()
{
    int socket_desc;
    struct sockaddr_in server;
    char server_reply[2000];
    char buff[1024];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    qDebug()<<"sock desc = "<<socket_desc;
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    bzero((char *)&server,sizeof(server));
    server.sin_addr.s_addr = inet_addr("115.111.229.10");//3");//"11.93.169.219"
    //http://111.93.169.219/pos/opr/pos.asmx?op=UpdateRealizedProper");
    server.sin_family = AF_INET;
    server.sin_port = htons(80 );

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return -1;
    }

    puts("Connected\n");

    memset(buff,0,sizeof(buff));
//	snprintf(buff,MAX,"POST %s",strlen(buffer),buffer);

    strcpy(buff, "/mnt/jffs2/outFile.csv");
    //Send some data
    // message = "GET / HTTP/1.1\r\n\r\n";
    if( send(socket_desc , buff , strlen(buff) , 0) < 0)
    {
        puts("Send failed");
        return -1;
    }
    puts("Data Send\n");

    memset(server_reply, 0, sizeof(server_reply));
    //Receive a reply from the server
    if( recv(socket_desc, server_reply , 2000 , 0) < 0)
    {
        puts("recv failed");
    }
    puts("Reply received\n");
    puts(server_reply);

    return 0;
}

int modem::Socket()
{
    char fname[64], buf[256];
    int s=0,c=0, bytes = 0;

    struct hostent *h;
    struct sockaddr_in channel; //ipv4 -> *ai_addr of channel

    printf("enter the file name : \t");
    scanf("%s",fname);

    h=gethostbyname("localhost");

    if(!h)
    {
        printf("\n gethostby name error");
        exit(0);
    }
    memset(&channel,0,sizeof(channel));
    channel.sin_family=AF_INET;
    memcpy(&channel.sin_addr.s_addr,h->h_addr,h->h_length);
    channel.sin_port=htons(SERVER_PORT);
    s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); //AF_INET = IPv4;
    if(s<0)
    {
        printf("socket error creation");
        exit(0);
    }
    c=connect(s, (struct sockaddr *)&channel,sizeof(channel));
    if(c<0)
    {
        printf("connection error");
        exit(0);
    }
    write(s,fname,strlen(fname));
    while(1)
    {
        bytes=read(s,buf,BUF_SIZE);
        if(bytes<=0)exit(0);
        write(1,buf,bytes);
    }
}

