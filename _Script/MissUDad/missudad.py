import os
import sys

# Define IP name you preferred.
IP_LIST=[ 'CRC' ]

# Define VCOM port number of NuMaker board.
COMX='COM6'

# Define running interval between SampleCodes. (Default: 20 seconds)
# Notice: The value depends on firmware flash time and SampleCode running time.
RUNTIME=20

# Define searching root directory
PROJ_FOLDER_NAME='../../../SampleCode'

'''
Define your NuEclipse/GDB/OpenOCD installation path.
'''
OPENOCD_EXE='C:\\Program Files (x86)\\Nuvoton Tools\\OpenOCD\\bin\\openocd_cmsis-dap.exe'
GDB_EXE='C:\\Program Files (x86)\\GNU Arm Embedded Toolchain\\10 2021.10\\bin\\arm-none-eabi-gdb.exe'
ECLIPSE_EXE="C:\\Program Files (x86)\\Nuvoton Tools\\NuEclipse\\V1.02.025c\\NuEclipse\\eclipse\\eclipsec.exe"

'''
Define your IAR embedded workbench installation path.
'''
CSPYBAT_EXE='C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\cspybat'
IARIDEPM_EXE='C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\IarIdePm.exe'
IARBUILD_EXE="C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\iarbuild.exe"

'''
Define your Keil v537 installation path.
'''
UV4_EXE="C:\\Keil_v537\\UV4\\Uv4.exe"


'''
Don't touch me. Just collect module function here.
'''
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
