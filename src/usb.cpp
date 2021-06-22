
#include "include/usb.h"
#include "include/misc.h"

USB::USB(QWidget *parent) : QDialog(parent)
{

}

USB::~USB()
{

}

int USB::USBMount()
{
    FILE *fp;

    char str[100]="",flag=0;
    int ret=0,mountcnt=0;

    gl11_host_pwron();
    sleep(1);

    fp = fopen("/etc/mtab","r");
    if(fp == NULL)
        qDebug() << "USB File open Error" << endl;

    while((fgets(str,80,fp)) != NULL)
    {
        if((strstr(str,"/mnt/usb")) != NULL)
            flag=1;
    }
    fclose(fp);


    qDebug()<<"Mounting USB.....";

    if(flag == 0)
    {
        sleep(2);
TRYMOUNT:

        if(mountcnt <= 5)
        {
            ret = system("mount -t vfat /dev/sda1 /mnt/usb");
            qDebug() << "ret =" << ret << endl;
            if(ret != 0)
                ret = system("mount -t vfat /dev/sdb1 /mnt/usb");
            if(ret != 0)
                ret = system("mount -t vfat /dev/sdc1 /mnt/usb");
            if(ret != 0)
                ret = system("mount -t vfat /dev/sdd1 /mnt/usb");

            if(ret != 0)
            {
                qDebug() << "############# TRYING AGAIN FOR USB MOUNTING: " << mountcnt << endl;
                sleep(2);
                mountcnt++;
                ret = -1;
                goto TRYMOUNT;
            }
        }
    }

    if(ret == 0)
        return 0;
    else
        return -1;

}

void USB::USBUnMount()
{
    qDebug()<<"USB Unmounting..........";
    system("umount /mnt/usb");
    sleep(2);
    gl11_host_pwroff();
    sleep(2);
}

int USB::DosToUnix(char *filename)
{
    return 0;

    char str[64], cmd[128];
    struct stat buf;

    memset(str, 0, sizeof(str));
    memset(cmd, 0, sizeof(cmd));

    int i = stat(filename, &buf );
    if((i<0) || (buf.st_size == 0))
    {
            printf("case 1: file not found\n");
            sprintf(str, "%s.dat", filename);
            i = stat (str, &buf );
            if((i<0) || (buf.st_size == 0))
            {
                printf("case 2: file not found\n");
                return -1;
            }
            memset(cmd, 0, sizeof(cmd));
            sprintf(cmd, "dos2unix %s", str);   //system("dos2unix /mnt/usb/kyc.dat");
            system(cmd);
            return 0;
    }
    return 0;
}

int USB::statFile(char *filename)
{
    struct stat buf;

    int i = stat(filename, &buf );
    if((i<0) || (buf.st_size == 0))
    {
        printf("file not found\n");
        return -1;
    }

    return 0;
}


int USB::IsMemorySufficientInUSB()
{
    char array1[64];
    memset(array1, 0, sizeof(array1));
    FILE *f1 = popen("du -c /mnt/jffs2/USB_EXPORT/ | grep total | awk '{print $1}'", "r");
    fgets(array1, sizeof(array1), f1);
    pclose(f1);

    if(atoi(array1) == 0 )
    {
        return -1;
    }

    char array2[64];
    memset(array2, 0, sizeof(array2));
    FILE *f2 = popen("free /mnt/usb/ | grep Total | awk '{print $4}'", "r");
    fgets(array2, sizeof(array2), f2);
    pclose(f2);

    qDebug()<<"Free Size       : "<< array2;

    if(atoi(array1) >= atoi(array2) )
    {
        return -2;
    }
    return 0;
}
