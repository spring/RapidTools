#!/bin/sh

SVNROOT=$(pwd)/test
PACKAGES=$(pwd)/packages
BIN=$(readlink -f $(pwd)/../Tools)
#URL=http://spring-features.googlecode.com/svn/trunk
URL=http://xta-springrts.googlecode.com/svn/trunk/
MODINFO=modinfo.lua
#LOGPATH=trunk/spring-features.sdd
LOGPATH=trunk
#TAG=spring-features
TAG=xta

svnadmin create "$SVNROOT"

 (
echo "#!/bin/sh"
echo 'set -e'
echo 'REPO="$1"'
echo 'REVISION="$2"'
echo "BIN=$BIN"
echo "echo \$BIN/BuildSvn file://\$REPO $LOGPATH $MODINFO $PACKAGES \$REVISION $TAG >> $SVNROOT/sync.log 2>&1"
echo "\$BIN/BuildSvn file://\$REPO $LOGPATH $MODINFO $PACKAGES \$REVISION $TAG >> $SVNROOT/sync.log 2>&1"
echo "#\$BIN/../Scripts/log.py \$REPO \$REVISION <channel1> <channel2>"
echo 'exit $?'
) > $SVNROOT/hooks/post-commit
chmod +x $SVNROOT/hooks/post-commit

(
echo '#!/bin/sh'
echo 'exit 0'
) > $SVNROOT/hooks/pre-revprop-change
chmod +x $SVNROOT/hooks/pre-revprop-change


echo svnsync init file://$SVNROOT $URL
svnsync init file://$SVNROOT $URL

echo Setup complete, to sync run:
echo svnsync sync file://$SVNROOT
