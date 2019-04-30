import serial
import time
import csv
import board
import busio
import adafruit_pca9685
from adafruit_servokit import ServoKit
kit = ServoKit(channels=16)
i2c = busio.I2C(board.SCL,board.SDA)
hat = adafruit_pca9685.PCA9685(i2c)

#### PIN NAMES ####
buzz = 2 # Buzzer
doorLvr = 3 # door lever
foodLvr = 4 # food lever
foodLED = 5
doorLED = 6
dispLED = 7

## Servos ##
doorServo = kit.servo[0] #Servo for door lever retraction
foodServo = kit.servo[1] #Servo for food lever retraction

#### USER DEFINED VARIABLES ####
# Section I
startTone = 880 # tone frequency
time1 = 2000 # 2 secs
# Section II
time2 = 10000
# Section III
pelletTone = 500
time3 = 1000
# Section IV
time4 = 2000
# Section V
time5 = 60000
