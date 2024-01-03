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

COMX='COM25'
IP_LIST=[  'ACMP',
            'CANFD',
            'DAC',
            'EADC',
            'HSUSBD',
            'LPADC',
            'LPUART',
            'UART',
            'USBD',
            'USCI_UART' ]

CSPYBAT_EXE='C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\cspybat'
PATH_IARIDEPMEXE='C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\IarIdePm.exe'
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
    err = 0
    root = os.getcwd()    
    os.chdir(root)

    f = open('iar.txt', "w")
    prj_count = 1

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):
        for file in fnmatch.filter(fileNames, '*.eww'):
            for ip in IP_LIST:
                if file.find(ip) == 0 :
                    os.chdir(dirPath)
                    prjName = os.path.splitext(file)[0]
                    if not os.path.isdir('settings'):
                        try:
                            buildcommnd = PATH_IARIDEPMEXE + " " + file
                            #print(buildcommnd)
                            p = subprocess.Popen(buildcommnd, startupinfo=si)
                            p.wait(10)
                        except subprocess.TimeoutExpired:
                            p.kill()
                        except OSError:
                            pass    #Silently ignore

                    try:
                        generalXclPath = os.path.abspath(os.path.join('settings', prjName + '.Release.general.xcl'))
                        driverXclPath  = os.path.abspath(os.path.join('settings', prjName + '.Release.driver.xcl'))
                        cspybatCmd = CSPYBAT_EXE + ' -f ' + generalXclPath + ' --backend -f ' + driverXclPath
                        #print(cspybatCmd)
                        print('#################################### '+prjName+' ####################################')
                        #p = subprocess.Popen(cspybatCmd, startupinfo=si, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
                        p = subprocess.Popen(cspybatCmd, startupinfo=si)
                        p.wait(120)
                    except subprocess.TimeoutExpired:
                        print('************************************ ' + prjName+' ************************************')
                        p.kill()
                    except OSError:
                        pass    #Silently ignore

                    prj_count += 1
                    f.flush()
                    os.chdir(root)

    if err == 0:
        f.write("Run " + str(prj_count) + " successfully.\n")

    f.close()

    if err == 0:
        sys.exit(0)
    else:
        sys.exit(1)