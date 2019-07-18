#!/usr/bin/python
#TCP全开端口扫描
import sys
from scapy.all import *
if len(sys.argv) != 3:
    print('Usage:Portscan <ip>\n eg: PortScan 192.168.1.1 80')#输出帮助信息
    sys.exit(1)
dst_ip = sys.argv[1]
src_port = RandShort()
dst_port = int(sys.argv[2])
packet = IP(dst=dst_ip)/TCP(sport=src_port,dport=dst_port,flags="S")#S表名自己是SYN请求数据包
resp = sr1(packet,timeout=10)#发送数据包并设置10秒超时
if(str(type(resp))=="type 'NoneType'>"):#如果resp为空，使用str判断是否为空，当type(resp)转化成type 'NoneType'则表名resp为空
    print "the port %s is closed" %( dst_port)
#判断是syn+ack，还是rst
elif (resp.haslayer(TCP)):#haslayer判断是否tcp
    if(resp.getlayer(TCP).flags == 0x12):#使用getlayer读取字段,0x12表示syn+ack
        sent_rst = sr(IP(dst=dst_ip)/TCP(sport=src_port,dport=dst_port,flags="AR"),timeout=10)#目标接收，发送ack数据包完成三次握手
        print "the port %s is open" %(dst_port)
    elif (resp.getlayer(TCP).flags == 0x14):#如果不是0x12,而是0x14也就是rst，表明端口是关闭的
        print "the port %s is closed" %(dst_port)
