

import turtle
from turtle import *
import serial

serialPort = serial.Serial('COM6', 9600)
list = [-1, -2, -3, -4]
serialString = ""  # Used to hold data coming over UART
turtle.speed('fastest')
# turtle.tracer(False)
i = 0
while 1:
    # if i > 1000:
    #     done()
    # i = i + 1
    # Wait until there is data waiting in the serial buffer
    if serialPort.in_waiting > 0:
        serialString = serialPort.readline()
        serialString = int(serialString.decode('Ascii'))
        print(serialString)


        if serialString not in list:
            penup()
            # forward(8 * 2)
            # pendown()
            if 2 < serialString < 350:
                forward(serialString * 2)
                pendown()
                dot(3, "red")
                penup()
                backward(serialString * 2)
            right(1)
            # penup()
            # backward(8 * 2)
            # pendown()
        else:
            left(180)
            if serialString == -3:
                penup()
                forward(5 * 2)
                pendown()
            elif serialString == -4:
                penup()
                backward(5 * 2)
                pendown()
            elif serialString == -2:
                left(90)
            elif serialString == -1:
                right(90)
