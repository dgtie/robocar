#!/usr/bin/python

import serial, struct

ser = serial.Serial('/dev/serial0', 115200)
while True:
  val = struct.unpack('<BBH', ser.read(4))
  print "{:08b}".format(val[2]), val
