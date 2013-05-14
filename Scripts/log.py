#!/usr/bin/python
import os
import re
import socket
import sys
import time

template = """
JOIN %(channel)s
SAY %(channel)s %(author)s commited revision %(revision)s
%(changed)s
SAY %(channel)s Log:
%(log)s

"""
# import the passwords from another file. this way we can keep config in git
import passwords

if len(sys.argv)<2:
	print("Usage: %s <repo> <revision> <channel1> <channel2>" %(sys.argv[0]))
	sys.exit(1)

# comment out to disable
#sys.exit(1)

repo = sys.argv[1]
revision = sys.argv[2]
channels = sys.argv[3:]

author = os.popen('svnlook author -r "%s" "%s"' % (revision, repo)).read()
changed = os.popen('svnlook changed -r "%s" "%s"' % (revision, repo)).read()
log = os.popen('svnlook log -r "%s" "%s"' % (revision, repo)).read()

# Strip empty lines
author = re.compile('\n+').sub('', author)
log = re.compile('\n+').sub('\n', log)

# Classify Changes
def makeChanges(channel):
    modified = []
    added = []
    deleted = []
    changedText = ''
    template = 'SAY %s     %%s' % channel

    for (changeType, changeFile) in re.compile('^(.)\s+(.+)$', re.M).findall(changed):
	if changeType == 'A':
            added.append(changeFile)
        elif changeType == 'U':
            modified.append(changeFile)
        elif changeType == 'D':
            deleted.append(changeFile)

    if len(added) > 0:
        added = [template % file for file in added]
        changedText += 'SAY %s Added:\n' % channel
        changedText += '\n'.join(added)
        changedText += '\n'

    if len(modified) > 0:
        modified = [template % file for file in modified]
        changedText += 'SAY %s Modified:\n' % channel
        changedText += '\n'.join(modified)
        changedText += '\n'

    if len(deleted) > 0:
        deleted = [template % file for file in deleted]
        changedText += 'SAY %s Deleted:\n' % channel
        changedText += '\n'.join(deleted)
        changedText += '\n'

    return changedText

def makeText(channel):
    logText = re.compile('^', re.M).sub('SAY %s     ' % channel, log)
    changedText = makeChanges(channel)
    return template % {"revision": revision,
                       "author": author,
                       "log": logText,
                       "channel": channel,
                       "changed": ""}


socket = socket.socket()
socket.connect(('lobby.springrts.com', 8200))
socket.sendall('LOGIN '+ passwords.username + ' ' + passwords.password + ' 2009 192.168.2.3 TASClient 0.33\n')

for channel in channels:
    text = makeText(channel)
    socket.sendall(text)

time.sleep(0.1)
socket.close()
