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

PROJ_FOLDER_NAME='../../../SampleCode'
PATH_IAREXE="C:\\Program Files\\IAR Systems\\Embedded Workbench 9.4\\common\\bin\\iarbuild.exe"
if __name__ == "__main__":
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    err = 0
    root = os.getcwd()    
    os.chdir(root)

    f = open('iar.txt', "w")
    prj_count = 1

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):
        for file in fnmatch.filter(fileNames, '*.ewp'):
            for ip in IP_LIST:
                if file.find(ip) == 0 :
                    os.chdir(dirPath)
                    try:
                        BUILDLOG = os.path.basename(file) + ".log"
                        fp = open(BUILDLOG, "w")

                        prjName = os.path.splitext(file)[0]
                        buildcommnd = PATH_IAREXE + " " + file + " -build * -log warnings"
                        print(buildcommnd)
                        subprocess.call(buildcommnd, startupinfo=si, stdout=fp, stderr=fp)
                        fp.flush()
                        fp.close()

                        # Find any error/warning
                        fp = open(BUILDLOG, "r")
                        lines = fp.readlines()
                        fp.close()

                        total_conf = 0
                        found = 0
                        prjNamePat = prjName + " - "
                        for line in lines:
                            if line.find(prjNamePat) >=0:
                                total_conf += 1
                                continue
                            if line.find("Total number of errors: 0") >=0:
                                found += 1
                                continue
                            if line.find("Total number of warnings: 0") >=0:
                                found += 1
                                continue

                        if total_conf > 0 and found != 2*total_conf:
                            err += 1
                            f.write("[" + str(prj_count) + "] "+ dirPath +  " has error or warning.\n")
                            print("Build " + file + " has error or warning...\n")
                        #else:
                            #f.write("[" + str(prj_count) + "] "+ os.path.abspath(file) +  " pass...\n")
                            #print("\t" + file +  " pass.\n")

                    except OSError:
                        pass    #Silently ignore

                    prj_count += 1
                    f.flush()
                    os.chdir(root)

    if err == 0:
        f.write("Build " + str(prj_count) + " projects successfully.\n")

    f.close()

    if err == 0:
        sys.exit(0)
    else:
        sys.exit(1)