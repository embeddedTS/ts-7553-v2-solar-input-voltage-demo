# ts-7553-v2-solar-input-voltage-demo
Code behind the TS-7553-V2 PowerFilm Solar trade show demo project.  Displays input voltage and TS-SILO charge on LCD display.

To build:

* apt-get install libcairo2-dev
* make
* make install
* cp solar.service /etc/systemd/system/
* systemctl enable solar.service
* systemctl start solar.service
