
#include "include/ethernet.h"


Ethernet::Ethernet()
{
     system("rm /etc/resolv.conf");
}

int Ethernet::STATIC()
{
    char ip_addr[60]="";
    char subnet_mask[60]="";
    char gateway[60]="";
    char pri_dns[60]="";
    char sec_dns[60]="";
    char dnsFileStr[50]="search localdomain\n";
    FILE *fp = NULL;

    QSqlQuery query;
    query.prepare("SELECT StaticIP, SubnetMask, GateWay, PrimaryDNS, SecondaryDNS FROM STATIC");
    if(!query.exec())
    {
        qDebug() << "In SELECT STATIC Info: " << query.lastError();
        return -1;
    }
    else
    {

        //gl11_eth_control(1);
        sleep(4);
        system("ifconfig eth0 up");
        sleep(1);
        while(query.next())
        {
            sprintf(ip_addr,"ifconfig eth0 %s up", query.value(0).toString().toAscii().constData());
            system(ip_addr);

            sprintf(subnet_mask,"ifconfig eth0 netmask %s up", query.value(1).toString().toAscii().constData());
            system(subnet_mask);

            sprintf(gateway,"route add default gw %s",query.value(2).toString().toAscii().constData());
            system(gateway);

            sprintf(pri_dns,"nameserver %s\n", query.value(3).toString().toAscii().constData());
            sprintf(sec_dns,"nameserver %s\n", query.value(4).toString().toAscii().constData());
        }
    }
    fp = fopen("/etc/resolv.conf","w+");
    if(fp == NULL)
    {
        qDebug()<<"resolv.conf failed to open";
        return -1;
    }
    fputs(dnsFileStr,fp);
    fputs(pri_dns,fp);
    fputs(sec_dns,fp);

    fclose(fp);

    return 0;
}


int Ethernet::DHCP()
{
    char str[75];
    int ethApi=0;
    int ret = 0;


    /*    ethApi=gl11_eth_control(1);
        printf("ETHERNET IN DHCP %d\n",ethApi);
        if(ethApi>=0)
        {*/
    //#ifdef GL11_GNOME_MICRON

    //ethApi=gl11_eth_control(1);
    //sleep(4);
    system("ifconfig eth0 up");
    sleep(1);
    printf("ETHERNET IN DHCP %d\n",ethApi);
    if(ethApi>=0)
    {
        ethApi=1;
        //gl11_eth_control(1);

        sprintf(str,"/sbin/udhcpc eth0 -T 10 -n"); //for exiting after 10 sec
        system(str);
        ret = action_ipView();


    }
    return -1;
}

int Ethernet::action_ipView()
{
    char *ip_addr;
    char *subnet_mask;
    char *gateway;
    char *pri_dns;
    char *sec_dns;
    qDebug()<<" Action Ip view";
    ip_addr = get_ip();
    qDebug()<<"Ipaddr"<<ip_addr;
    if(strcmp(ip_addr,"NO IP") != 0)//Changed from != to == by Tr
    {
        qDebug()<<"IP addr = "<<ip_addr;
        return 0;
    }
    qDebug()<<"\nNO IP Generated";
    return -1;

    QString(QLatin1String(ip_addr));

    subnet_mask= get_subnet();
    if(strcmp(subnet_mask,"No Subnet") == 0)
    {
        qDebug()<<"NO subnet mask Generated";
        return -1;
    }//Conditioned by Tr
    QString(QLatin1String(subnet_mask));

    gateway = get_gateway();
    if(strcmp(gateway,"No Gateway") == 0)
    {
        qDebug()<<"NO gw Generated";
        return -1;
    }//Conditioned by Tr
    QString(QLatin1String(gateway));

    pri_dns = get_pri_dns();
    if(strcmp(pri_dns,"NO Primary DNS") == 0)
    {
        qDebug()<<"NO pri dns Generated";
        return -1;
    }//Conditioned by Tr

    QString(QLatin1String(pri_dns));

    sec_dns = get_sec_dns();
    if(strcmp(sec_dns,"No Secondary") == 0)
    {
        qDebug()<<"NO sec dns Generated";
        return -1;
    }//Conditioned by Tr
    QString(QLatin1String(sec_dns));


    return 0;

}


char* Ethernet::get_ip()
{
      int fd;
      char str[40]="";
      struct ifreq ifr;
      int ret;
      qDebug()<<"Get Ip";
      fd=socket(AF_INET,SOCK_DGRAM,0);
      printf("fd===%d\n",fd);
      ifr.ifr_addr.sa_family = AF_INET;
      strncpy(ifr.ifr_name, "eth0"  /*ethtype*/ ,IFNAMSIZ-1);
      ret=ioctl(fd,SIOCGIFADDR,&ifr);
      printf("ret===%d\n",ret);
      ::close(fd);
      if(ret==-1)
          return "NO IP";

      sprintf(str,"%d.%d.%d.%d",(unsigned char)ifr.ifr_addr.sa_data[2],
                                (unsigned char)ifr.ifr_addr.sa_data[3],
                                (unsigned char)ifr.ifr_addr.sa_data[4],
                                (unsigned char)ifr.ifr_addr.sa_data[5]);

      /*printf("%d.%d.%d.%d",(unsigned char)ifr.ifr_addr.sa_data[2],
                                (unsigned char)ifr.ifr_addr.sa_data[3],
                                (unsigned char)ifr.ifr_addr.sa_data[4],
                                (unsigned char)ifr.ifr_addr.sa_data[5]);*/
      qDebug()<<"IP: "<<str;
      if(strcmp(str,"NULL")!=0)
      {
          memset(ipInfostr, 0, sizeof(ipInfostr));
        strcpy(ipInfostr,str);
        return ipInfostr;
      }
      else
          return "No IP";

}

char* Ethernet::get_gateway()
{
    FILE *fp;
    int count;
    char str[50],str1[50]="Not found";
    system ("route | grep default > /tmp/dfgw");
    fp=fopen("/tmp/dfgw","r");
    if(fp==NULL)
    fclose(fp);
    else
     {
       for(count=0;count<10;count++)
        {
          fscanf(fp,"%s",str);
          if(fp==NULL)
            break;
          else if(!(strcmp(str,"default")))
           {
            fscanf(fp,"%s",str1);
            break;
           }
       }
        fclose(fp);
        remove("/tmp/dfgw");
     }
    if(strcmp(str1,"NULL")!=0)
    {
        memset(ipInfostr, 0, sizeof(ipInfostr));
     strcpy(ipInfostr,str1);
     return ipInfostr;
    }
    else
        return "No Gateway";

}

char* Ethernet::get_subnet()
{
     char ethtype[30]="eth0";
     char str[40]="";
     int fd;
     struct ifreq ifr;
     fd=socket(AF_INET,SOCK_DGRAM,0);
     ifr.ifr_addr.sa_family = AF_INET;
     strncpy(ifr.ifr_name, ethtype,IFNAMSIZ-1);
     ioctl(fd,SIOCGIFNETMASK,&ifr);
     ::close(fd);
     sprintf(str,"%d.%d.%d.%d",(unsigned char)ifr.ifr_netmask.sa_data[2],
                               (unsigned char)ifr.ifr_netmask.sa_data[3],
                               (unsigned char)ifr.ifr_netmask.sa_data[4],
                               (unsigned char)ifr.ifr_netmask.sa_data[5]);

     if(strcmp(str,"NULL")!=0)
     {
         memset(ipInfostr, 0, sizeof(ipInfostr));
     strcpy(ipInfostr,str);
     return ipInfostr;
     }
     else
         return "No Subnet";
}


char* Ethernet::get_pri_dns()
{
      FILE *fp;
      int count;
      char str[80]="",str1[50]="Not Found";
      fp = fopen("/etc/resolv.conf","r");
      if(fp==NULL)
          fclose(fp);
      else
         for(count=0;count<20;count++)
         {
               fscanf(fp,"%s",str);

               if(!strcmp(str,"nameserver"))
               {
                  fscanf(fp,"%s",str1);
                  fclose(fp);
                  break;
               }
         }
      if(strcmp(str1,"NULL")!=0)
      {

          memset(ipInfostr, 0, sizeof(ipInfostr));
     strcpy(ipInfostr,str1);
     return ipInfostr;
      }
      else
          return "NO Primary DNS";

}


char* Ethernet::get_sec_dns()
{
    FILE *fp;
    char str[80]="",str1[50]="Not Found";
    int flag=0,count;
    fp = fopen("/etc/resolv.conf","r");
    if(fp==NULL)
    fclose(fp);
    else
    for(count=0;count<50;count++)
    {
         fscanf(fp,"%s",str);
         if(!(strcmp(str,"nameserver")))
          {
            if(flag==1)
               {
                 fscanf(fp,"%s",str1);
                 fclose(fp);
                 break;
               }
           else
           flag =1;
          }
     }
    if(strcmp(str1,"NULL")!=0)
    {

        memset(ipInfostr, 0, sizeof(ipInfostr));
     strcpy(ipInfostr,str1);
     return ipInfostr;
    }
    else
        return "No Secondary";
}


