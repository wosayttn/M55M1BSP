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
PATH_UV4="C:\\Keil_v537\\UV4\\Uv4.exe"

if __name__ == "__main__":
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    err = 0
    root = os.getcwd()
    f = open('keil.txt', "w+")

    os.chdir(root)

    prj_count = 1

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):

        for file in fnmatch.filter(fileNames, '*.uvprojx'):

            for ip in IP_LIST:

                if file.find(ip) == 0 :

                    os.chdir(dirPath)

                    try:
                        BUILDLOG = file + ".log"
                        buildcommnd = PATH_UV4 + " -b -j0 -z -o " + BUILDLOG + " " + file
                        cleancommnd = PATH_UV4 + " -j0 -c " + file

                        # https://www.keil.com/support/man/docs/uv4cl/uv4cl_commandline.htm
                        # -j0   Hides the µVision GUI. Messages are suppressed. Use this option for batch testing.
                        # -z    Re-builds all targets of a project or multiple-project.
                        #       Ensure that each target has another object output folder.
                        #       Use the menu Projects - Options for Target - Output - Select Folder for Objects.
                        # -b    Builds the last current target of a project and exits after the build process finished.
                        #       Refer to option -t to change the target.
                        #       For multi-projects, the command builds the targets as defined in the dialog Project - Batch Build.
                        # -o outputfile
                        #f.write("[" + str(prj_count) + "] Build " + os.path.abspath(file) +  "\n")

                        print("[" + str(prj_count) + "] "+ os.getcwd() + "\\" + file +  " cleaning.\n")
                        subprocess.call(cleancommnd, startupinfo=si, stdout=f, stderr=f)

                        print("[" + str(prj_count) + "] "+ os.getcwd() + "\\" + file +  " building.\n")
                        subprocess.call(buildcommnd, startupinfo=si, stdout=f, stderr=f)

                        # It's a bit strange keil report error code as 0 even build failed. so parse k.log
                        tmp = open(BUILDLOG, "r")
                        lines = tmp.readlines()
                        tmp.close()
                        #os.remove(BUILDLOG)
                        found = 0
                        for line in lines:
                            if line.find("0 Error(s), 0 Warning(s)") >= 0:
                                found = 1
                        if found == 0:
                            err += 1
                            f.write("[" + str(prj_count) + "] "+ os.path.abspath(file) +  " has error or warning.\n")
                            #print("\t" + os.getcwd() + "\\" + file +  " has error or warning.\n")
                        #else:
                            #f.write("[" + str(prj_count) + "] "+ os.path.abspath(file) +  " pass...\n")
                            #print("\t" + os.getcwd() + "\\" + file +  " pass.\n")

                    except Exception as e:
                        f.write("[" + str(prj_count) + "] "+ "Build " + file +  " has error or warning.\n")
                        print("[" + str(prj_count) + "] "+ "Build" + file +  "has error or warning.\n")
                        err += 1
                    except OSError:
                        #f.write("[" + str(prj_count) + "] " + os.path.abspath(file) + "Ooops\n") ##
                        pass #Silently ignore

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
