import os
import sys

IP_LIST=[ 'CRC' ]
PROJ_FOLDER_NAME='../../../SampleCode'
COMX='COM6'
RUNTIME=10

# GCC
OPENOCD_EXE='C:\\Program Files (x86)\\Nuvoton Tools\\OpenOCD\\bin\\openocd_cmsis-dap.exe'
GDB_EXE='C:\\Program Files (x86)\\GNU Arm Embedded Toolchain\\10 2021.10\\bin\\arm-none-eabi-gdb.exe'
ECLIPSE_EXE="C:\\Program Files (x86)\\Nuvoton Tools\\NuEclipse\\V1.02.025c\\NuEclipse\\eclipse\\eclipsec.exe"

# IAR
CSPYBAT_EXE='C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\cspybat'
IARIDEPM_EXE='C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\IarIdePm.exe'
IARBUILD_EXE="C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\iarbuild.exe"

# KEIL
UV4_EXE="C:\\Keil_v537\\UV4\\Uv4.exe"

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
