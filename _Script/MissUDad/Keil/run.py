import os
import sys
import time
import subprocess
import shutil
import fnmatch
import tempfile
import glob
import re
import sys
import datetime
import serial
import threading
import time

COMX='COM6'
IP_LIST=[   'ACMP',
            'CANFD',
            'DAC',
            'EADC',
            'HSUSBD',
            'LPADC',
            'LPUART',
            'UART',
            'USBD',
            'USCI_UART' ]

PATH_UV4="C:\\Keil_v537\\UV4\\Uv4.exe"
PROJ_FOLDER_NAME='../../../SampleCode'


def read_serial_port(serial_port):
    while True:
        if serial_port.in_waiting > 0:
            data = serial_port.readline().decode('utf-8').strip()
            print(data)

def write_to_serial_port(serial_port):
    while True:
        message = input('$ ')
        serial_port.write(message.encode('utf-8'))
        print(message)

# Replace 'COMx' with the actual serial port on your system, e.g., 'COM1' or '/dev/ttyUSB0'
serial_port = serial.Serial(COMX, 115200, timeout=1)

# Create and start the read and write threads
read_thread = threading.Thread(target=read_serial_port, args=(serial_port,))
write_thread = threading.Thread(target=write_to_serial_port, args=(serial_port,))

read_thread.start()
write_thread.start()

if __name__ == "__main__":
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW

    root = os.getcwd()

    os.chdir(root)

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):
        for file in fnmatch.filter(fileNames, '*.uvprojx'):
            for ip in IP_LIST:
                if file.find(ip) == 0 :
                    os.chdir(dirPath)
                    prjName = os.path.splitext(file)[0]
                    try:
                        RunCmd = PATH_UV4 + ' -j0 -f ' + file
                        print(RunCmd)
                        print('#################################### '+prjName+' ####################################')
                        #p = subprocess.Popen(RunCmd, startupinfo=si, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
                        p = subprocess.Popen(RunCmd, startupinfo=si)
                        p.wait(10)
                    except subprocess.TimeoutExpired:
                        print('************************************ ' + prjName+' ************************************')
                        p.kill()
                    except OSError:
                        pass    #Silently ignore
                    os.chdir(root)
