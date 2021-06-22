
#include "include/bluetooth.h"
#include "include/hci.h"
#include "include/hci_lib.h"

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>


#define SUCCESS 0
#define FAILURE -1
#define ERROR -2

int usb_node_check(char *usb_node)
{
    int ret=0;

    ret = system("udevadm info --name=/dev/ttyUSB6 --attribute-walk | grep 0403");
    strcpy(usb_node,"/dev/ttyUSB0");
    if(ret != 0)
    {
        ret = system("udevadm info --name=/dev/ttyUSB0 --attribute-walk | grep 0403");
        strcpy(usb_node,"/dev/ttyUSB1");
        if(ret !=0)
        {
            ret = system("udevadm info --name=/dev/ttyUSB1 --attribute-walk | grep 0403");
            strcpy(usb_node,"/dev/ttyUSB4");
            if(ret !=0)
            {
                ret = system("udevadm info --name=/dev/ttyUSB2 --attribute-walk | grep 0403");
                strcpy(usb_node,"/dev/ttyUSB2");
                if(ret !=0)
                {
                    ret = system("udevadm info --name=/dev/ttyUSB3 --attribute-walk | grep 0403");
                    strcpy(usb_node,"/dev/ttyUSB3");
                    if(ret !=0)
                    {
                        ret = system("udevadm info --name=/dev/ttyUSB4 --attribute-walk | grep 0403");
                        strcpy(usb_node,"/dev/ttyUSB4");
                        if(ret !=0)
                        {
                            ret = system("udevadm info --name=/dev/ttyUSB5 --attribute-walk | grep 0403");
                            strcpy(usb_node,"/dev/ttyUSB5");
                            if(ret !=0)
                            {
                                printf("No USB node Found\n");
                                return -1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int usb_node_check_Essae(char *usb_node)
{
    int ret=0;
    char buff[128];
    int flag = 0;
    int i =0;
    for(i = 0; i < 8; i++)
    {
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "udevadm info --name=/dev/ttyUSB%d --attribute-walk | grep 6015", i);
        ret = system(buff);
        if(ret == 0)
        {
            sprintf(usb_node, "/dev/ttyUSB%d", i);
            flag = 1;
            break;
        }
    }
    if(flag)
        return 0;
    else
        return -1;

    return 0;
}


int data_weight(char *node,char *data)
{
    struct termios my_termios;
    int Baudrate=0;
    char *ptr; char buff[20]="";
    int i = 0, m = 0;
    unsigned char recv[40]="";
    static int fd;

    memset(&my_termios, 0, sizeof(struct termios));

    Baudrate=B9600;
    fd = open(node,O_RDWR|O_NOCTTY|O_NONBLOCK);
    if( fd < 0 || Baudrate ==0 )
    {
        fprintf(stderr,"Unable to open port\n");
        return -1;
    }

    tcflush(fd,TCIFLUSH);
    my_termios.c_cflag=Baudrate|CS8|CREAD;//|PARENB;
    my_termios.c_cc[VMIN]=0;
    my_termios.c_cc[VTIME]=0;
    tcsetattr(fd,TCSANOW,&my_termios);

    memset(recv,0x00,40);
    sleep(1);
jump:
    m = read(fd,&recv,40);
    if(m > 0)
    {
        if(((ptr = strchr(recv,'-'))!=NULL) || ((ptr = strchr(recv,'+')) != NULL))
        {
            while(i<11)
            {
                buff[i]=ptr[i];
                i++;
            }
        }
        else
            goto jump;

        strcpy(data,buff);
    }
    else
    {
        close(fd);
        return -2;
    }

    close(fd);
    return 0;
}


int bt_connect(char *bt_id)
{
    char buffer[64]="";
    int ret=0;

    sprintf(buffer, "rfcomm connect 0 %s 1 &", bt_id);
    ret = system(buffer);
    sleep(5);
    ret = system("ls /dev/rfcomm0 > /dev/null");
    if(ret != 0)
        return -1;

    return 0;
}

int bt_reconnect(char *bt_id)
{
    //char bt_id[16];
    int ret = 0;

    ret = bt_node_chk();
    if(ret != 0)
    {
        while(1)
        {
            ret = bt_connect(bt_id);
            if(ret < 0)
            {
                fprintf(stdout, "In bt_reconnect(), bt_connect() Failed...\n");
                return -1;
            }

        }
    }

    return 0;
}


int bt_scan(char *bt_id)
{
    FILE *fp;
    int rd;
    char *line=NULL,*str;
    int len=20;
    char buff[20]="";
    int i=0;

    system("hcitool scan > /mnt/jffs2/scaning.txt");
    fp = fopen("/mnt/jffs2/scaning.txt","r");
    if(fp == NULL)
    {
        printf("unable to open scaning file \n");
        return -2;
    }

    while(rd = getline(&line, &len, fp) !=NULL)
    {
        if((str = (char *)strstr(line,"APPDS_VNTK@2015")) != NULL)
        {
            line=line+1;
            strncpy(buff,line,17);
            printf("bt_id==%s\n",buff);
            strcpy(bt_id,buff);
            fclose(fp);
            return 0;
        }
        i++;
        if(i>20)
            break;
    }
    return -1;
}

int scanning(char bt_names[40][256])
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[256] = { 0 };


    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if ((hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),name, 0)) < 0)
        {
            strcpy(name, "[unknown]");
        }
        strcat(name," ");
        strcat(name,addr);
        memset(*(bt_names + i),0,256);
        strcpy(*(bt_names + i),name);
    }

    free( ii );
    close( sock );

    return num_rsp;
}

int bt_node_chk()
{
    int ret;
    ret = system("ls /dev/rfcomm0 > /dev/null");
    if(ret != 0)
    {
        return -1;
    }

    return 0;
}

int bt_pwr_on()
{
    FILE *fp, *fp1;
    int ret=0,i;
    char buff1[100]="",*chr;
    char buffer[50]="";

    system("lsusb > /mnt/jffs2/lsusb.txt");
    fp1 = fopen("/mnt/jffs2/lsusb.txt","r");
    fseek(fp1,0,2);
    ret = ftell(fp1);
    rewind(fp1);
    char *ch=(char *)malloc(ret);
    for(i=0;i<ret;i++)
    {
        ch[i]=fgetc(fp1);
    }
    chr = strstr(ch,"0a12:0001");//94321
    if(chr == NULL)
    {
        printf("\t\tBluetooth Device Failure \n");
        system("rm -f /mnt/jffs2/lsusb.txt");
        fclose(fp1);
        return ERROR;
    }
    fclose(fp1);
    printf("\t\tBluetooth Device Success \n");

    system("hciconfig > /mnt/jffs2/hciconfig.txt");
    fp = fopen("/mnt/jffs2/hciconfig.txt","r");
    if( fp == NULL )
    {
        printf("hciconfig file not found\n");
        return -1;

    }

    fread(buffer,30,1,fp);
    chr = strstr(buffer,"hci0:");
    if(chr == NULL)
    {
        printf("\t\t Bluetooth poweron Failed ... \n\n");
        return FAILURE;
    }
    sprintf(buff1,"/etc/rc.d/init.d/bluetooth restart");
    ret = system(buff1);
    sleep(1);
    system("rm -f /mnt/jffs2/hciconfig.txt");
    system("rm -f /mnt/jffs2/lsusb.txt");
    fclose(fp);
    free(ch);

    return SUCCESS;

}

int bt_pwroff()
{
    int ret=0, i;
    char buff[64];

    system("rfcomm release 0");
    sleep(3);


    memset(buff, 0, sizeof(buff));
    sprintf(buff,"/etc/rc.d/init.d/bluetooth stop");
    ret = system(buff);
    if(ret != 0 )
    {
        fprintf(stderr,"\t\t Stopping Bluetooth Service Failure\n\n");
        return FAILURE;
    }

    return SUCCESS;
}

int bt_paring(char *client_btaddr, char *passkey)
{
    FILE *fp;
    int ret,i,j;
    char buff[128];
    char details[64];

    system("rfcomm release 0");
    sleep(3);

    memset(details, 0, sizeof(details));

    memset(buff, 0, sizeof(buff));
    sprintf(buff,"simple-agent %s %s > /mnt/jffs2/client_paring.txt",client_btaddr,passkey);
    system(buff);

    fp = fopen("/mnt/jffs2/client_paring.txt","r");
    fseek(fp,0,2);
    ret = ftell(fp);
    rewind(fp);
    for(i=0;i<ret;i++)
    {
        details[i]=fgetc(fp);
    }
    if(strstr(details,"Pairing failed:"))
    {
        fclose(fp);
        return -1;
    }
    else if(strstr(details,"Pairing sucessfull"))
    {
        fclose(fp);
        return 0;
    }
}

int getweight(char *btaddr, char *data)
{
    int ret=0 ,run=0;
    int count = 0;
    int FLAG=0;

reconnect:

    run = bt_node_chk();
    if(run != 0  || ret != 0)
    {
        ret = bt_connect(btaddr);
        if(ret !=0)
        {
            printf("node creation and connection failure \n");
            system("rfcomm release 0");
            sleep(1);

            return -1;
        }

        sleep(2);
    }

    if(count < 10)
    {
        memset(data, 0, sizeof(data));

        ret = data_weight("/dev/rfcomm0",data);
        if(ret !=0)
        {
            printf("obtaining data  failure \n");

            run = bt_node_chk();
            if(run != 0  || ret != 0)
            {
                FLAG=1;
                system("rfcomm release 0");
                sleep(1);
            }
        }

        printf("weight = %s\n",data);
    }
    else
    {
        return -1;
    }

    if(FLAG == 1 )
    {
        count++;
        goto reconnect;
    }

    return 0;
}

