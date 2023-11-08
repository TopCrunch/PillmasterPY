from pynput import keyboard
from pynput.keyboard import Key
import os
import serial
import platform

clear = lambda: os.system('cls') #on windows only
SERIAL_PORT_WIN = "COM4"
SERIAL_PORT_LNX = "/dev/ttyACM0"

if(platform.system().lower() == 'windows'):
    ser = serial.Serial(SERIAL_PORT_WIN, 9600)
else:
    ser = serial.Serial(SERIAL_PORT_LNX, 9600)

ary = [0] * 10
sel = [''] * 5
mptr = 0
altPressed = False

PAD_MTR = 5
PAD_FWD = 16
PAD_BOTH = 32
PAD_ADJ = 48
PAD_REV = 64

def operate(x):
    ser.write(chr(mptr+PAD_MTR+x).encode('utf-8'))

def incptr():
    global mptr
    if(mptr >= 4):
        mptr = 0
    else:
        mptr += 1

def decptr():
    global mptr
    if(mptr <= 0):
        mptr = 4
    else:
        mptr -= 1    

def incdeg(x):
    global mptr
    if x == False:
        ary[mptr*2] += 18
    ary[(mptr*2)+1] += 18
    
    if(ary[mptr*2] >= 360):
        ary[mptr*2] = 0
    if(ary[(mptr*2)+1] >= 360):
        ary[(mptr*2)+1] = 0
        
def decdeg(x):
    global mptr
    if x == False:
        ary[mptr*2] -= 18
    ary[(mptr*2)+1] -= 18
    
    if(ary[mptr*2] < 0):
        ary[mptr*2] = 342
    if(ary[(mptr*2)+1] < 0):
        ary[(mptr*2)+1] = 342

def rightKey(x):
    #print("Right key clicked")
    incdeg(x)
    if(x == True):
        operate(PAD_ADJ)
    else:
        operate(PAD_FWD)
def leftKey(x):
    #print("Left key clicked")
    decdeg(x)
    if(x == True):
        operate(PAD_BOTH)
    else:
        operate(PAD_REV)
def upKey():
    #print("Up key clicked")
    decptr()
def downKey():
    #print("Down key clicked")
    incptr()

def loadASCII():
    clear()
    
    print(' __________________')
    print('/ MOTOR ID | ANGLE \\')
    print('====================')
    if(mptr == 0 and altPressed == False):
        print('|=> A_TOP  |  ' + str(ary[0]).zfill(3) +'  |')    
    else:
        print('|   A_TOP  |  ' + str(ary[0]).zfill(3) +'  |')
    if(mptr == 0 and altPressed == True):
        print('|=> A_BTM  |  ' + str(ary[1]).zfill(3) +'  |')
    else:
        print('|   A_BTM  |  ' + str(ary[1]).zfill(3) +'  |')
    print('====================')
    if(mptr == 1 and altPressed == False):
        print('|=> B_TOP  |  ' + str(ary[2]).zfill(3) +'  |')
    else:
        print('|   B_TOP  |  ' + str(ary[2]).zfill(3) +'  |')
    if(mptr == 1 and altPressed == True):
        print('|=> B_BTM  |  ' + str(ary[3]).zfill(3) +'  |')
    else:
        print('|   B_BTM  |  ' + str(ary[3]).zfill(3) +'  |')
    print('====================')
    if(mptr == 2 and altPressed == False):
        print('|=> C_TOP  |  ' + str(ary[4]).zfill(3) +'  |')
    else:
        print('|   C_TOP  |  ' + str(ary[4]).zfill(3) +'  |')
    if(mptr == 2 and altPressed == True):
        print('|=> C_BTM  |  ' + str(ary[5]).zfill(3) +'  |')
    else:
        print('|   C_BTM  |  ' + str(ary[5]).zfill(3) +'  |')
    print('====================')
    if(mptr == 3 and altPressed == False):
        print('|=> D_TOP  |  ' + str(ary[6]).zfill(3) +'  |')
    else:
        print('|   D_TOP  |  ' + str(ary[6]).zfill(3) +'  |')
    if(mptr == 3 and altPressed == True):
        print('|=> D_BTM  |  ' + str(ary[7]).zfill(3) +'  |')
    else:
        print('|   D_BTM  |  ' + str(ary[7]).zfill(3) +'  |')
    print('====================')
    if(mptr == 4 and altPressed == False):
        print('|=> E_TOP  |  ' + str(ary[8]).zfill(3) +'  |')
    else:
        print('|   E_TOP  |  ' + str(ary[8]).zfill(3) +'  |')
    if(mptr == 4 and altPressed == True):
        print('|=> E_BTM  |  ' + str(ary[9]).zfill(3) +'  |')
    else:
        print('|   E_BTM  |  ' + str(ary[9]).zfill(3) +'  |')
    print('====================\n')

loadASCII()

def on_key_release(key):
    global altPressed
    if altPressed == False:
        if key == Key.right:
            rightKey(False)
        elif key == Key.left:
            leftKey(False)
        elif key == Key.up:
            upKey()
        elif key == Key.down:
            downKey()
    else:
        if key == Key.right:
            rightKey(True)
        elif key == Key.left:
            leftKey(True)
        elif key == Key.up:
            upKey()
        elif key == Key.down:
            downKey()
    if key == Key.esc:
        exit()
    
    loadASCII()


def on_key_pressed(key):
    global altPressed
    if key == Key.alt_l:
        altPressed = not altPressed

with keyboard.Listener(on_release=on_key_release, on_press=on_key_pressed) as listener:
    listener.join()