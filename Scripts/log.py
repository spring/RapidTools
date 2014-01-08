#!/usr/bin/python
import os
import re
import socket
import sys
import time

if len(sys.argv)<2:
	print("Usage: %s <repo> <revision> <channel1> <channel2>" %(sys.argv[0]))
	sys.exit(1)

# comment out to disable
#sys.exit(1)
try:
	import passwords
	username = passwords.username
	password = passwords.username
except:
	env = os.environ
	username = os.environ['LOBBY_USERNAME']
	password = os.environ['LOBBY_PASSWORD']


repo = sys.argv[1]
revision = sys.argv[2]
channels = set(sys.argv[3:])

if not os.path.exists(repo):
	print("Repo %s doesn't exist!" %(repo))
	sys.exit(1)
author = os.popen('svnlook author -r "%s" "%s"' % (revision, repo)).read().strip()
log = os.popen('svnlook log -r "%s" "%s"' % (revision, repo)).read().strip()
#changed = os.popen('svnlook changed -r "%s" "%s"' % (revision, repo)).read().strip()

def createTemplate(str):
	lines = str.split('\n')
	template = "JOIN $CHANNEL$\n"
	for line in lines:
		line = line.strip()
		if len(line) > 0:
			template += "SAY $CHANNEL$ " + line + "\n"
	return template

template = createTemplate("%s commited revision %s:\n%s" %(author, revision, log))


buf = 'LOGIN ' + username + ' ' + password + ' 0 * TASClient 0.33\t0\tcl sp\n'
for channel in channels:
	buf += template.replace("$CHANNEL$", channel)
buf += "EXIT Thanks for using rapid! https://github.com/spring/RapidTools\n"

socket = socket.socket()
socket.settimeout(5)
socket.connect(('lobby.springrts.com', 8200))
socket.sendall(buf)
socket.close()

