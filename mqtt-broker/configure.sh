#!/bin/bash

#  ____  ____      _    __  __  ____ ___
# |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
# | | | | |_) |  / _ \ | |\/| | |  | | | |
# | |_| |  _ <  / ___ \| |  | | |__| |_| |
# |____/|_| \_\/_/   \_\_|  |_|\____\___/
#                           research group
#                             dramco.be/
#
#  KU Leuven - Technology Campus Gent,
#  Gebroeders De Smetstraat 1,
#  B-9000 Gent, Belgium
#
#         File: configure.sh
#       Author: Gilles Callebaut
#
#  Description: Install MQTT and config when fresh install
#		- cofigures new hostname
#		- adds duckdns domainname
#		- installs and configures firewall
#		- installs moqsuitto
#

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

echo "Running configuration for MQTT-server @ PI"

echo "Updated and upgrading"
apt-get update
apt-get upgrade

echo "Installing latest ssh server"
apt install openssh-server
echo "Installing latest avahi-daemon"
apt-get install avahi-daemon


echo "Configuring hostname:"
#Assign existing hostname to $hostn
hostn=$(cat /etc/hostname)

#Display existing hostname
echo "Existing hostname is $hostn"

#Ask for new hostname $newhost
echo "Enter new hostname: "
read newhost

#change hostname in /etc/hosts & /etc/hostname
sed -i "s/$hostn/$newhost/g" /etc/hosts
sed -i "s/$hostn/$newhost/g" /etc/hostname

#display new hostname
echo "Your new hostname is $newhost"

service hostname.sh stop
service hostname.sh start

echo "Configuring duckdns"
cd /home/$newuser
mkdir duckdns
cd duckdns
echo "Enter domainname: "
read name
echo "Enter domaintoken: "
read token
touch duckdns.sh
echo -e "echo url=\"https://www.duckdns.org/update?domains=${name}&token=${token}&ip=\" | curl -k -o ~/duckdns/duck.log -K -" >> duckdns.sh
chmod 700 duckdns.sh
echo "*/5 * * * * ~/duckdns/duck.sh >/dev/null 2>&1" >> /etc/crontab


echo "Installing firewall"
apt-get install ufw
ufw default deny incoming
ufw default allow outgoing
echo "Allowing ssh, MQTT and MQTT over websockets"
ufw allow ssh
ufw allow 8883 # allow MQTT
ufw allow 8083 # allow over websockets
ufw enable
echo "Firewall status:"
ufw status verbose

echo "Installing Mosquitto"
wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key
apt-key add mosquitto-repo.gpg.key
touch /etc/apt/sources.list.d/mosquitto.list
echo "deb http://repo.mosquitto.org/debian stretch main" >> /etc/apt/sources.list.d/mosquitto.list
apt-get update
aptitude install mosquitto mosquitto-clients


read -s -n 1 -p "Press any key to reboot"
echo -e "Allright ${newuser}, see you back at ${newhost}.local or ${name}."
sleep 3s
reboot
