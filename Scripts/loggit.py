#!/usr/bin/python
import os
import socket
import sys
import time

if len(sys.argv)<2:
	print("Usage: echo msg | %s <channel1> <channel2>" %(sys.argv[0]))
	sys.exit(1)


# comment out to disable
import imp
passwords = imp.load_source("passwords", os.path.join(os.path.dirname(__file__), "passwords.py"))
username = passwords.username
password = passwords.password

channels = set(sys.argv[1:])

template = "JOIN $CHANNEL$\n"
for line in sys.stdin:
	template += "SAY $CHANNEL$ " + line + "\n"
buf = 'LOGIN ' + username + ' ' + password + ' 0 * TASClient 0.33\t0\tcl sp p\n'
for channel in channels:
	buf += template.replace("$CHANNEL$", channel)
buf += "EXIT Thanks for using rapid! https://github.com/spring/RapidTools\n"

socket = socket.socket()
socket.settimeout(2.0)
socket.connect(('lobby.springrts.com', 8200))
socket.sendall(buf)

time.sleep(1.0)
socket.close()

