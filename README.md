# nrf9160-aqi

This repo contains all the source code for our bachelor thesis called: 
>"_Prototype of a cloud-connected monitoring system for EAQI, based on the nRF9160 SiP with an energy harvesting system_"

We are 4 students (_now through_) studying Electronics Enginering (FTHINGEL) at NTNU in Trondheim, Norway. This thesis has been developed in collaboration with Nordic Semiconductor, where we were tasked with building a prototype of a weather station that was based on their chip, nRF9160, with an energy harvesting system. This system should also be able to send data to a cloud service.

This repo contains all of the source code from the thesis, including the sensor drivers for the air quality sensors, based on the nRF environment, and the Leshan/database code that we used for cloud services.

**NB**: We will add a link here to the thesis once it has been through the publishing system at NTNU ;)

## Installation

What follows is the different installation commands for the source code in this repo:

Installing the repo locally
```sh
git clone https://github.com/patricab/nrf9160-aqi.git
```

Install PostgreSQL database
```sh
echo "deb http://apt.postgresql.org/pub/repos/apt/ $(lsb_release -c -s)-pgdg main" |
sudo tee /etc/apt/sources.list.d/pgdg.list
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc |
sudo apt-key add -
sudo apt update
sudo apt install postgresql postgresql-contrib # install postgresql and extensions
sudo -u postgres psql -c "SELECT version();"
# ‘lsb_release -c -s‘ should return the correct codename of your OS
sudo sh -c "echo ’deb https://packagecloud.io/timescale/timescaledb/debian/
‘lsb_release -c -s‘ main’ > /etc/apt/sources.list.d/timescaledb.list"
wget --quiet -O - https://packagecloud.io/timescale/timescaledb/gpgkey |
sudo apt-key add - # Add repository
sudo apt update
sudo apt install timescaledb-2-postgresql-13 # install appropriate package
sudo su - postgres # login as postgres
psql # start database
sudo su - postgres -c "createuser airq" # create user
sudo su - postgres -c "crearedb airqdb" # create database
sudo -u postgres psql # enter database
GRANT ALL PRIVILEGES ON DATABASE airqdb TO airq; # grant privileges
```

=======
Install Grafana server
```sh
sudo apt install -y apt-transport-https
sudo apt install -y software-properties-common wget
wget -q -O - https://packages.grafana.com/gpg.key |
sudo apt-key add - # add grafana repository Open-Source Software version
echo "deb https://packages.grafana.com/oss/deb stable main" |
sudo tee -a /etc/apt/sources.list.d/grafana.list
sudo apt update # update package list
sudo apt install grafana # install grafana
sudo service grafana-server start # Start grafana server
sudo service grafana-server status # Check if it works
sudo update-rc.d grafana-server defaults # Set to start on boot

```

## Use

Building the code for Thingy:91
```sh
west build -b thingy91_nrf9160ns -p -- -DOVERLAY_CONFIG=overlay-queue.conf
```

Building the code for the nRF9160DK
```sh
west build -b nrf9160dk_nrf9160ns -p -- -DOVERLAY_CONFIG=overlay-queue.conf
```
** (Note that for the nRF9160DK, Kconfig settings _CONFIG_LWM2M_IPSO_TEMP_SENSOR_, _CONFIG_LWM2M_IPSO_HUMIDITY_SENSOR_ and _CONFIG_LWM2M_IPSO_CONC_SENSOR_ needs to turned off) **

Start Leshan server
```sh
cd nrf9160-aqi/leshan/aqi-server
mvn install
java -jar target/aqi-server-1.0-SNAPSHOT-jar-with-dependencies.jar
```

Run test client
```sh
cd nrf9160-aqi/leshan/thingy-sim
mvn install
java -jar target/thingy-sim-1.0-SNAPSHOT-jar-with-dependencies.jar
```

## Support
If you have any problems or grievances with this project, then you can raise a ticket in the _Issues_ tab here on GitHub, and we will try to answer you as soon as we can :)

## License
This project uses a 5-clause BSD license developed by Nordic Semiconductor. See the [License]{LICENSE} for further information.

