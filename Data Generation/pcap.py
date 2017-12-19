import os
from pcapfile import savefile
from pcapfile.protocols.linklayer import ethernet
from pcapfile.protocols.network import ip
import binascii

entity = 0
fp = open('data.data','w')
base = '/home/vijay/Downloads/pcap-files'

for f1 in os.listdir(base) :
    for f2 in os.listdir(os.path.join(base,f1)): 
        cls = 0
        for f3 in os.listdir(os.path.join(base,f1,f2)) : 
            testcap = open(os.path.join(base,f1,f2,f3),'rb')
            capfile = savefile.load_savefile(testcap)
            entity += 1
            cls += 1
            for packet in capfile.packets :
                eth_frame = ethernet.Ethernet(packet.raw())
                ip_packet = ip.IP(binascii.unhexlify(eth_frame.payload))
                direction = 0
                if ip_packet.src == '10.10.10.10' :
                    direction = 1
                fp.write(str(direction) + " " + str(ip_packet.len) + " " + str(cls) + " " + str(entity) + "\n")
        print entity

fp.close()
print "total entities : " + str(entity)
