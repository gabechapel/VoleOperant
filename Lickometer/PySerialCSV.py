# requires the following python libraries:
#  pyserial

import serial
import csv

### Setup the serial port
# change this string depending upon where your computer makes a device for the Arduino
serialport = "/dev/cu.usbmodem1411"
ser = serial.Serial(serialport, 9600)

### Setup the CSV file
name = input("Filename ")
dataFile = open(name+'.csv',mode='w')

while(True):
    line = ser.readline().decode('utf8').strip()
    dataFile.write(line + '\n')
    print(line)