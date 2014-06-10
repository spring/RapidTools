#!/bin/bash


DOMAINS=$(find /home/packages/git -maxdepth 1 -mindepth 1 -type d)

for DOMAIN in $DOMAINS; do

TAG=$(basename $DOMAIN)

# 001- is prefixed for defined order of files
FILE=/etc/apache2/sites-enabled/001-$TAG-repo.springrts.com
echo "Creating $FILE"
cat >$FILE << EOF

#Automaticly created with $(pwd)/$0, don't edit!
<VirtualHost *:80>
    ServerName $TAG-repo.springrts.com
    ServerAdmin chris@springrts.com
    ErrorLog /var/log/apache2/$TAG-repo.springrts.com-error.log
    CustomLog /var/log/apache2/$TAG-repo.springrts.com-access.log combined
    DocumentRoot /home/packages/www/$TAG

    <Location />
        RewriteEngine on
        RewriteRule streamer.cgi$ nph-streamer2.cgi
        AddHandler cgi-script .cgi
        Options +ExecCGI
    </Location>
</VirtualHost>
EOF

done
