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
PATH_ECLIPSE="C:\\Program Files (x86)\\Nuvoton Tools\\NuEclipse\\V1.02.025c\\NuEclipse\\eclipse\\eclipsec.exe"

if __name__ == "__main__":
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    err = 0
    root = os.getcwd()
    f = open('gcc.txt', "w")
   
    prj_count = 1

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):

        for file in fnmatch.filter(fileNames, '*.cproject'):

            basename = os.path.basename(os.path.dirname(dirPath))

            prjName = basename
            
            for ip in IP_LIST:

                if prjName.find(ip) == 0 :

                    if os.path.isdir('Temp'):
                        shutil.rmtree('Temp')

                    os.mkdir('Temp')
                    buildcommnd = PATH_ECLIPSE + " -nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data Temp -import " + dirPath + " -build all"

                    BUILDLOG = os.path.join(dirPath, basename) + '.log'

                    try:
                        print(dirPath + " building ...")
                        fp = open(BUILDLOG, "w")
                        subprocess.check_call(buildcommnd, startupinfo=si, stdout=fp, stderr=fp)
                        fp.flush()
                        fp.close()

                        # Find any error/warning
                        fp = open(BUILDLOG, "r")
                        lines = fp.readlines()
                        fp.close()

                        found = 0
                        for line in lines:
                            if line.find(" error: ") >= 0 or line.find(" warning: ") >= 0:
                                found = 1

                        if found == 1:
                            err += 1
                            f.write("[" + str(prj_count) + "] "+ dirPath +  " has error or warning.\n")
                            LIST_MAIL_ATTACHMENT.append(str(os.path.abspath(BUILDLOG)))
                            #print("Build " + basename + " has error or warning...\n")
                        #else:
                            #f.write("[" + str(prj_count) + "] "+ os.path.abspath(file) +  " pass...\n")
                            #print("\t" + basename +  " pass.\n")
                        pass
                    except Exception as e:
                        f.write("[" + str(prj_count) + "] "+ dirPath +  " has error or warning.\n")
                        LIST_MAIL_ATTACHMENT.append((os.path.abspath(BUILDLOG)))
                        #print("Build" + file +  "has error or warning...\n")
                        err += 1
                    except OSError:
                        f.write("Ooops\n")  ##
                        pass                # Silently ignore

                    prj_count += 1
                    f.flush()

    if os.path.isdir('Temp'):
        shutil.rmtree('Temp')

    if err == 0:
        f.write("Build " + str(prj_count) + " projects successfully.\n")

    f.close()

    if err == 0:
        sys.exit(0)
    else:
        sys.exit(1)