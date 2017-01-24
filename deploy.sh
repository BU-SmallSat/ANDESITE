#/bin/bash
#go root; it'll be easier. 
sudo bin/bash
cd ~
#turn on networking if it doesn't already exist, and update the time server
/sbin/route add default gw 192.168.7.1
ntpdate pool.ntp.org
#update cache
apt-get update

#upgrade what we have 
apt-get upgrade -y

# we need python3.5
apt-get -y install build-essential libncursesw5-dev libgdbm-dev libc6-dev
apt-get -y install zlib1g-dev libsqlite3-dev tk-dev
apt-get -y install libssl-dev openssl
#gotta do it from source!
curl -Ok https://www.python.org/ftp/python/3.5.2/Python-3.5.2.tgz
tar -zxvf Python-3.5.2.tgz
cd Python-3.5.2
./configure
make
make install
cd ~

# optional: install pycharm helpers for development
#"/usr/local/bin/python3.5" "/root/.pycharm_helpers/pydev/setup_cython.py" build_ext --inplace

# we need python 3 packages now
pip3.5 install pyserial
pip3.5 install pexpect

#get ADAFruit_BBIO working with a total hack
apt-get -y install libi2c-dev
wget http://ftp.de.debian.org/debian/pool/main/i/i2c-tools/i2c-tools_3.1.0.orig.tar.bz2
tar xf i2c-tools_3.1.0.orig.tar.bz2
cd i2c-tools-3.1.0/py-smbus/
mv smbusmodule.c smbusmodule.c.orig
wget https://gist.githubusercontent.com/sebastianludwig/c648a9e06c0dc2264fbd/raw/2b74f9e72bbdffe298ce02214be8ea1c20aa290f/smbusmodule.c
python3.5 setup.py build
python3.5 setup.py install

cd ~
git clone git://github.com/adafruit/adafruit-beaglebone-io-python.git
cd adafruit-beaglebone-io-python/
python3.5 setup.py install 

# now we can install the MULE. 
