#!/usr/bin/python3
import time
import socket

UDP_IP = "192.168.137.106"
UDP_PORT = 57222

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

alpha_set = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K'}

def speaker():
    sock.sendto("Speaker".encode(), (UDP_IP, UDP_PORT))

def freq(f):
    send_me = "s_" + (str(f).zfill(4))
    sock.sendto(send_me.encode(), (UDP_IP, UDP_PORT))

def test():
    sock.sendto("Test".encode(), (UDP_IP, UDP_PORT))

def manual():
    sock.sendto("Manual".encode(), (UDP_IP, UDP_PORT))

def auto():
    sock.sendto("Auto".encode(), (UDP_IP, UDP_PORT))

def led(val):
    send_me = "LED_" + val
    sock.sendto(send_me.encode(), (UDP_IP, UDP_PORT))

def RGB(r, g, b):
    send_me = "c_" + str(r).zfill(3) + " " + str(g).zfill(3) + " " + str(b).zfill(3)
    sock.sendto(send_me.encode(), (UDP_IP, UDP_PORT))

def BIG(r, g, b):
    send_me = "m_" + str(r).zfill(3) + " " + str(g).zfill(3) + " " + str(b).zfill(3)
    sock.sendto(send_me.encode(), (UDP_IP, UDP_PORT))

def dim(val, num):
    send_me = "Dim_" + val + " " + str(num).zfill(3)
    sock.sendto(send_me.encode(), (UDP_IP, UDP_PORT))

if __name__ == "__main__":
    auto()
    manual()
    speaker()
    time.sleep(0.5)
    freq(1000)
    time.sleep(0.5)
    speaker()
    RGB(100, 255, 255)
    BIG(255, 255, 255)
    for c in alpha_set:
        led(c)
        for i in range(256, -1, -8):
            dim(c, i)
            time.sleep(0.1)
        for i in range(0, 257, 8):
            dim(c, i)
            time.sleep(0.1)
        led(c)
    speaker()
    time.sleep(0.5)
    freq(1000)
    time.sleep(0.5)
    speaker()
    auto()
    test()
    print("All done!")


