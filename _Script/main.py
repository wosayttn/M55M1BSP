# NOTE: This script supports Python 3.x only, and haven't been tested on Python 2.x

__copyright__ = "Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved"
__version__ = "v2.12.15"

import os
import sys
import time
import subprocess
import shutil
import fnmatch
import tempfile
import glob
import re
import xml.etree.ElementTree as ET # phone home :p
import mmap
import configparser

#Define for Doxygen
MAIN_HEADER_NAME = "NuMicro.h"                                                          # File needs convert before execute Doxygen
MAIN_HEADER_PATH = "Library\\Device\\Nuvoton\\M55M1\\Include\\"                         # Path to main header file

#Keyword for project check
NULINK_CHIPNAME = "M55M1"                                                               # Chip name defined in Nu_Link_Driver.ini
NULINK_RESET = "Autodetect"                                                             # Reset defined in Nu_Link_Driver.ini
KEIL_DEVICE = { "M55M1H2LJAE" }                                                         # Device name defined in Keil project file
KEIL_ICE = "Bin\\Nu_Link.dll"                                                           # ICE setting for debug and download in Keil project file
IAR_DRIVER = "$TOOLKIT_DIR$\\..\\..\\..\\Nuvoton Tools\\Nu-Link_IAR\\Nu-Link_IAR.dll"   # Use relative path for NuLink IAR driver
IAR_DEVICE = "M55M1 series\tNuvoton M55M1 series"                                       # Device name in IAR project file
IAR_RSTHANDLER = "Reset_Handler"                                                        # Replace default project handler
GCC_CHIPSERIES = "NuMicro M55"                                                          # Chip series defined in preferences.ini
GCC_TARGETCHIP = "0x43"                                                                 # Target chip defined in preferences.ini

# Execute Astyle prettifier, needs to take special care for *.c and *.h with assembly code inside, for example retarget.c
def astyle():
    f = open('astyle.txt', "w+")
    subprocess.call('\"C:\\Program Files (x86)\\AStyle\\bin\\AStyle.exe\" \
                    --options=_Script\\AStyle_BSP.txt --ascii --recursive \
                    *.c,*.h',
                    shell=True, stdout=f, stderr=f)
    f.close()
    print("Done...")
    time.sleep(3)

# Clean up BSP directory. Since this script is executing, _script directory need remove manually
def cleanup():
    rmfile_list = {"vssver2.scc", "vssver.scc", "*.bak", "*.YACHEN", ".CHHSIEH3", "*.dep", "*.tra", "*.doc", "*.sfr", "*.docx", "*.old"}
    rmdir_list = {"obj", "lst", "Release", "setting"}
    print("Shouldn't rely on this function to do the clean up before release BSP")
    print("Archive by git command instead.")
    choice = input("Press y/Y to proceed, other keys to abort")
    if choice not in ['y', 'Y']:
        return
    print("Cleaning up...")
    # remove common files and directories
    for dirPath, dirNames, fileNames in os.walk('.'):
        for dir in dirNames:
            if dir in  rmdir_list:
                shutil.rmtree(os.path.join(dirPath, dir))
        for rmfile in rmfile_list:
            for file in fnmatch.filter(fileNames, rmfile):
                os.remove(os.path.join(dirPath, file))

    # remove smartcard library source file
    # note that rmtree cannot handle read only files in Python 3.4 and needs an error handler for this
    shutil.rmtree('Library\\SmartcardLib\\Source')
    # remove DSRC driver and sample code
    os.remove('Library\\StdDriver\\src\\dsrc.c')
    os.remove('Library\\StdDriver\\inc\\dsrc.h')
    shutil.rmtree('SampleCode\\StdDriver\\DSRC_Control')
    print("Done. Do not forget to remove _script directory manually!!")
    time.sleep(3)


string1 = '\n\\htmlonly\r\n<html><table class=\"fixed\" border=\"1\" style=\"border-collapse:collapse;\" borderColor=black ><col width=\"75px\" /><col width=\"125px\" /><col width=\"700px\" /><caption align=\"left\"><font size=\"3\">'
string2 = '</font><br><p> <font size=\"2\">'
string3 = '</font></caption><thread><tr bgcolor=\"#8A0808\" ><td><font color=white><b>Bits</b></font></td><td><font color=white><b>Field</b></font></td><td><font color=white><b>Descriptions</b></font></td></tr></thread><tbody>'
string4 = '<tr><td>'
string5 = '</td><td>'
string6 = '</td><td><div style=\"word-wrap: break-word;\"><b>'
string7 = '</div></td></tr>'
string8 = '</tbody></table></html>\r\n\\endhtmlonly\r\n'

def create_manual():

    print("Converting header file")

    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW

    shutil.copytree(MAIN_HEADER_PATH, "_Include")



    for dirPath, dirNames, fileNames in os.walk(MAIN_HEADER_PATH):
        for file in fnmatch.filter(fileNames, '*_reg.h'):

            cell = 0   # Output cell
            nread = 1  # Need to read in bit loop?
            end = 0    # End of reg group
            ifile = open(MAIN_HEADER_PATH + file, "r")
            ofile = open("temp.h", "w+")
            while True:
                if nread == 1:
                    line = ifile.readline()
                    if not line: # EOF
                        break
                else:
                    nread = 1

                if line[0:12] == "     * @var ": # found the first line of register definition
                    found = line.find("::")
                    # output structure and register name
                    ofile.write(line[7:len(line)] + string1)
                    # output register name into table, the 2 is to skip "::"
                    ofile.write(line[found + 2:len(line)] + string2 + "\n")

                    line = ifile.readline()

                    # output offset and description
                    ofile.write(line[7:len(line)] + string3 + "\n")

                    # //* ---------------------------------------------------------------------------------------------------
                    # //* |Bits    |Field     |Descriptions
                    # //* | :----: | :----:   | :---- |
                    # above are three lines we don't need, skip them
                    line = ifile.readline()
                    line = ifile.readline()
                    line = ifile.readline()

                    cell = 0

                    while True :
                        line = ifile.readline()

                        if len(line) > 12 :
                            if line[0:12] == "     * @var " : #end of current register entry
                                nread = 0
                                break

                        if line[0:7] == "     */": #end of current register table comment block
                            end = 1
                            break

                        if line[8] == '[': # begin of bit field, contains offset, name, and description
                            if cell == 1:
                                ofile.write(string7)
                            cell = 1
                            ofile.write(string4 + "\n")

                            found = line.find("]", 8)
                            ofile.write(line[8 : found + 1] + string5)

                            found = line.find("|", found)
                            found += 1

                            while True:
                                ofile.write(line[found])    # name, some registers have longer name, so need to check "|"
                                found += 1
                                if line[found] == "|" or line[found] == " " :
                                    break

                            ofile.write(string6)

                            while line[found] != "|" :
                                found += 1
                            found += 1

                            ofile.write(line[found:len(line)] + "</b><br>\n")

                        else:
                            ofile.write(line[28:len(line)] + "<br>\n")

                    ofile.write(string7 + string8 + "\n")


                    if end:
                        ofile.write("\n */\n")
                        end = 0

                else:
                    ofile.write(line)

            ifile.close()
            ofile.close()
            shutil.move("temp.h", MAIN_HEADER_PATH + file)

    print("Executing Doxygen")
    try :
        subprocess.run("C:\\Program Files\\doxygen\\doxygen.exe _scripts\\Doxyfile.txt", startupinfo=si)
    #except :
    #    pass #Silently ignore error message
    except Exception as e:
        print(e)
        raise
    for file in glob.glob(r"Document\\html\\*.chm"):
        shutil.copy(file, "Document")
    print("Clean up")
    try:
        shutil.rmtree('Document\\html')
    except Exception as e:
        print(e)
        raise
    # restore header file
    shutil.rmtree(MAIN_HEADER_PATH)
    shutil.move("_Include", MAIN_HEADER_PATH)


    # remove information we don't want
    f = open('doxy.txt', "r")
    lines = f.readlines()
    f.close()
    f = open("doxy.txt","w")

    for line in lines:
        l = line.upper()
        if l.find("[Warning] ARGUMENT 'NONE' OF COMMAND") < 0:  # we don't care these warning message
            f.write(line)
    f.close()

    print("Done...")
    time.sleep(3)

# Test build all IAR projects within BSP directory. Build result write to iar_log.txt. Need to check if there's error/warning.
def build_iar():
    f = open('iar_log.txt', "w+")
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    root = os.getcwd()

    for dirPath, dirNames, fileNames in os.walk('SampleCode'):
        for file in fnmatch.filter(fileNames, '*.ewp'):
            print('Building', file)
            os.chdir(dirPath)
            try:
                subprocess.call("C:\\Program Files (x86)\\IAR Systems\\Embedded Workbench 8.2\\common\\bin\\iarbuild.exe " + file + " -build Release -log warnings",
                                startupinfo=si, stdout=f, stderr=f)
            except OSError:
                pass #Silently ignore
            os.chdir(root)
    f.close()

    # iar output is a little messy, now clean it up
    f = open('iar_log.txt', "r")
    lines = f.readlines()
    f.close()
    f = open("iar.txt","w")
    found = 0
    for line in lines:
        if line.find("Building configuration") >= 0:
            if found != 0:
                index1 = line1.find(":") + 2 # ignore ": "
                index2 = line1.find(" ", index1)
                f.write("Build " + line1[index1:index2] + " has error or warning...\n")
            line1 = line
            found = 0
        if line.find("Total number of") >= 0:
            if line.find("Total number of errors: 0") >= 0:
                continue
            if line.find("Total number of warnings: 0") >= 0:
                continue
            found = 1
    f.close()
    print("Build IAR done...")
    time.sleep(3)

# Test build all Keil projects within BSP directory. Report to keil.txt if any error/warning found.
def build_keil():
    # need to check project file missing??
    f = open('keil.txt', "w+")
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    root = os.getcwd()

    for dirPath, dirNames, fileNames in os.walk('SampleCode'):
        for file in fnmatch.filter(fileNames, '*.uvprojx'):
            print('Building', file)
            os.chdir(dirPath)
            try:
                f1 = open(file, 'r+')
                mm = mmap.mmap(f1.fileno(), 0)
            except:
                #printf("Parse " + file + " failed\n")
                f1.close()
                pass
            else:
                # disable cross module optimization to reduce build time
                pos = mm.find(b'<OptFeed>1</OptFeed>')
                if pos >= 0:
                    mm.seek(pos + 9)
                    mm.write(b"0")
                mm.close()
                f1.close()
                    
            try:
                subprocess.check_call("C:\\Keil_v5\\UV4\\Uv4.exe -b -j0 -z " + file, startupinfo=si, stdout=f, stderr=f)
            except OSError:
                pass #Silently ignore
            except Exception as e:
                f.write("Build " + file + " has error or warning...\n")
            os.chdir(root)
    f.close()
    print("Build Keil done...")
    time.sleep(3)

# Test build all GCC projects within BSP directory. Report to gcc.txt if any error/warning found.
def build_gcc():
    # Need to check project file missing??
    f = open('gcc_log.txt', "w+")
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW

    for dirPath, dirNames, fileNames in os.walk('SampleCode'):
        for file in fnmatch.filter(fileNames, '*.cproject'):
            print('Building', dirPath + '\\' + file)
            if os.path.isdir("Temp"):
                shutil.rmtree("Temp")
            os.mkdir("Temp")
            try:
                subprocess.check_call("C:\\Program Files (x86)\\Nuvoton Tools\\NuEclipse\\V1.01.015\\NuEclipse\\eclipse\\eclipsec.exe " +
                                      "-nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild " +
                                      "-data Temp -import " + dirPath + " -build all",
                                      startupinfo=si, stdout=f, stderr=f)
            except Exception as e:
                pass #Silently ignore
            except OSError:
                pass #Silently ignore
            shutil.rmtree("Temp")
    f.close()

    # Find any error/warning
    f = open('gcc_log.txt', "r")
    lines = f.readlines()
    f.close()
    f = open("gcc.txt", "w")
    for line in lines:
        if line.find(" for project ") >= 0:
            index1 = line.find(" for project ") + 13
            index2 = line.find(" ****", index1)
            prjName = line[index1:index2]
        elif line.find(": error: ") >= 0 or line.find(": warning: ") >= 0:
            f.write("Build " + prjName + " has error or warning...\n")
    f.close()
    print("Build GCC done...")
    time.sleep(3)

# Fix GCC folder structure
def fix_gcc_folder():
    for dirPath, dirNames, fileNames in os.walk('SampleCode'):
        for dir in dirNames:
            if dir.upper() == 'GCC':
                if os.path.isdir(os.path.join(dirPath, 'GCC', '.metadata')):
                    shutil.rmtree(dirPath + "\\GCC\\.metadata")
                prj_cnt = len(glob.glob(dirPath + r"\\GCC\\*\\.project"))
                idx = [m.start() for m in re.finditer(r"\\", dirPath)]
                dirname = dirPath[idx[-1] + 1 :]
                for subd in next(os.walk(os.path.join(dirPath, 'GCC')))[1]:  # check directory child only
                    if prj_cnt == 1 or subd == dirname:
                        for file in os.listdir(os.path.join(dirPath, 'GCC', subd)):
                            if file == ".cproject":
                                ifile = open(dirPath + "\\GCC\\" + subd + "\\.cproject", "r")
                                ofile = open(dirPath + "\\GCC\\.cproject", "w")
                                isDriverSample = dirPath.find("StdDriver") or dirPath.find("_Internal")
                                while True:
                                    line = ifile.readline()
                                    if not line: # EOF
                                        break
                                    if isDriverSample >= 0:
                                        line = line.replace("${ProjDirPath}/../", "${ProjDirPath}/")
                                        ofile.write(line.replace("${ProjDirPath}\\..\\", "${ProjDirPath}\\"))
                                    else:
                                        line = line.replace("${ProjDirPath}/../", "${ProjDirPath}/")
                                        ofile.write(line.replace("${ProjDirPath}\\..\\", "${ProjDirPath}\\"))
                                ifile.close()
                                ofile.close()
                            elif file == ".project":
                                ifile = open(dirPath + "\\GCC\\" + subd + "\\.project", "r")
                                ofile = open(dirPath + "\\GCC\\.project", "w")
                                while True:
                                    line = ifile.readline()
                                    if not line: # EOF
                                        break
                                    line = line.replace("<locationURI>PARENT-2-PROJECT_LOC", "<locationURI>PARENT-1-PROJECT_LOC")
                                    line = line.replace("<locationURI>PARENT-3-PROJECT_LOC", "<locationURI>PARENT-2-PROJECT_LOC")
                                    line = line.replace("<locationURI>PARENT-4-PROJECT_LOC", "<locationURI>PARENT-3-PROJECT_LOC")
                                    line = line.replace("<locationURI>PARENT-5-PROJECT_LOC", "<locationURI>PARENT-4-PROJECT_LOC")
                                    ofile.write(line.replace("<locationURI>PARENT-6-PROJECT_LOC", "<locationURI>PARENT-5-PROJECT_LOC"))
                                ifile.close()
                                ofile.close()
                            elif os.path.isfile(os.path.join(dirPath, 'GCC', subd, file)):
                                shutil.copy(dirPath + "\\GCC\\" + subd + "\\" + file, dirPath + "\\GCC\\" + file)
                        shutil.rmtree(dirPath + "\\GCC\\" + subd)
                    else:
                        print("[Warning] Keep " + dirPath + "\\GCC\\" + subd)
    print("Done...")
    time.sleep(10)

# Check project settings
def check_prj():
    debugLevel = 0  # 0/1 to disable/enable

    f = open('checkprj.txt', "w+")
    for dirPath, dirNames, fileNames in os.walk('SampleCode'):
        idx = [m.start() for m in re.finditer(r"\\", dirPath)]
        if len(idx) < 1:
            continue

        # Find GCC project folder
        dirname = dirPath[idx[-1] + 1: ]
        if len(idx) > 1:
            dirname2 = dirPath[idx[-2] + 1: idx[-1]]
        else:
            dirname2 = dirPath[0: idx[0]]
        if dirname.upper() == 'GCC' or dirname2.upper() == 'GCC':
            if dirname.upper() == 'GCC':
                if len(dirNames) > 0:
                    for dir in dirNames:
                        if dir.find(dirname2) != 0:
                            f.write("[Warning] " + os.path.join(dirPath, dir) + ": Project name is not prefixed by sample name (" + dirname2 + ").\n")
                    continue    # Check .project/preferences.ini in sub-folder
                dirname = dirname2

            # Check GCC project setting
            file = os.path.join(dirPath, '.project')
            if os.path.isfile(file) == False:
                f.write("[Error] " + dirPath + ": .project is missing.\n")
            else:
                if debugLevel:
                    print("Checking " + file)
                tree = ET.parse(file)
                root = tree.getroot()
                elem = root.find('name')
                if elem.text.find(dirname) != 0:
                    sys.stdout.write('.')
                    sys.stdout.flush()
                    f.write("[Auto-Fixed] " + file + ": Project name (" + elem.text + ") differs from sample name (" + dirname + ").\n")
                    elem.text = dirname
                    tree.write(file, "UTF-8", True)
                try:
                    project = open(file, 'r+')
                    mm = mmap.mmap(project.fileno(), 0)
                except:
                    f.write("[Error] Failed to read " + file + "\n")
                else:
                    pos = mm.find(b'\\')
                    if pos >= 0:
                        f.write("[Warning] " + file + ": Found backslash.\n")
                mm.close()
                project.close()

            file = os.path.join(dirPath, 'preferences.ini')
            if os.path.isfile(file) == False:
                f.write("[Error] " + dirPath + ": preferences.ini is missing.\n")
            else:
                if debugLevel:
                    print("Checking " + file)
                config = configparser.ConfigParser()
                config.read(file)
                if config.get('startup', 'chipSeries') != GCC_CHIPSERIES:
                    f.write("[Error] " + file + ": Chip series (" + config.get('startup', 'chipSeries') + ") is not " + GCC_CHIPSERIES + ".\n")
                if config.get('startup', 'targetChip') != GCC_TARGETCHIP:
                    f.write("[Error] " + file + ": Target chip (" + config.get('startup', 'targetChip') + ") is not " + GCC_TARGETCHIP + ".\n")

            file = os.path.join(dirPath, '.cproject')
            if os.path.isfile(file) == False:
                f.write("[Error] " + dirPath + ": .cproject is missing.\n")
            else:
                if debugLevel:
                    print("Checking " + file)
                try:
                    cproject = open(file, 'r+')
                    mm = mmap.mmap(cproject.fileno(), 0)
                except:
                    f.write("[Error] Failed to read " + file + "\n")
                else:
                    pos = mm.find(b'.usenewlibnano.')
                    if pos < 0:
                        f.write("[Warning] " + dirPath + ": Not use recommended newlib-nano.\n")
                    pos = mm.find(b'\\')
                    if pos >= 0:
                        f.write("[Warning] " + file + ": Found backslash, invalid in Linux file path.\n")
                mm.close()
                cproject.close()
 
        # Check KEIL project setting
        # Check Nu_Link_Driver.ini exist and ChipName setting is correct
        for dir in dirNames:
            if dir.upper() == 'KEIL':
                prj_cnt = len(glob.glob(dirPath + r"\\Keil" + r"\\*.uvprojx"))
                if prj_cnt == 0:
                    f.write("[Error] " + os.path.join(dirPath, 'Keil') + ": uvprojx file is missing.\n")
                elif prj_cnt > 1:
                    f.write("[Warning] " + os.path.join(dirPath, 'Keil') + ": More than one uvprojx file.\n")
                cnt = len(glob.glob(dirPath + r"\\Keil" + r"\\*.uvoptx"))
                if cnt == 0 or cnt < prj_cnt:
                    f.write("[Error] " + os.path.join(dirPath, 'Keil') + ": uvoptx file is missing.\n")
                elif cnt > 1:
                    f.write("[Warning] " + os.path.join(dirPath, 'Keil') + ": More than one uvoptx file.\n")
                file = os.path.join(dirPath, 'Keil', 'Nu_Link_Driver.ini')
                if os.path.isfile(file) == False:
                    f.write("[Error] " + os.path.join(dirPath, 'Keil') + ": Nu_Link_Driver.ini is missing.\n")
                else:
                    if debugLevel:
                        print("Checking " + file)
                    config = configparser.ConfigParser()
                    config.read(file)
                    if config.get('ChipSelect', 'ChipName') != NULINK_CHIPNAME:
                        f.write("[Error] " + file + ": ChipName (" + config.get('ChipSelect', 'ChipName') + ") is not " + NULINK_CHIPNAME + ".\n")
                    elif config.get(NULINK_CHIPNAME, 'Reset') != NULINK_RESET:
                        f.write("[Error] " + file + ": [" + NULINK_CHIPNAME + "] Reset (" + config.get(NULINK_CHIPNAME, 'Reset') + ") is not " + NULINK_RESET + ".\n")

        for file in fnmatch.filter(fileNames, '*.uvprojx'):
            file_modified = 0
            # compare project name with sample name
            filename = file.partition(".")[0]
            idx = [m.start() for m in re.finditer(r"\\", dirPath)]
            dirname = dirPath[idx[-2] + 1 : idx[-1]]
            if prj_cnt == 1 and filename != dirname:
                sys.stdout.write('.')
                sys.stdout.flush()
                f.write("[Auto-Fixed] " + os.path.join(dirPath, file) + ": uvprojx name differs from sample name (" + dirname + ").\n")
                os.rename(os.path.join(dirPath, file), os.path.join(dirPath, dirname + ".uvprojx"))
                file = dirname + ".uvprojx"
                filename = dirname
            elif prj_cnt > 1 and filename.find(dirname) != 0:
                f.write("[Warning] " + os.path.join(dirPath, file) + ": uvprojx name is not prefixed by sample name (" + dirname + ").\n")
            # check device setting
            if debugLevel:
                print("Checking " + os.path.join(dirPath, file))
            tree = ET.parse(os.path.join(dirPath, file))
            root = tree.getroot()
            elems = root.find('Targets')
            # check each target
            for elem in elems.findall('Target'):
                targetname = elem.find('TargetName')
                if targetname.text.find(filename) != 0:
                    #sys.stdout.write('.')
                    #sys.stdout.flush()
                    f.write("[Warning] " + os.path.join(dirPath, file) + ": Target name (" + targetname.text + ") is not the same or prefixed by project name (" + filename + ").\n")
                    #targetname.text = filename
                    #file_modified = 1
                elem = elem.find('TargetOption')
                elem1 = elem.find('TargetArmAds')
                elem2 = elem.find('DebugOption')
                elem3 = elem.find('Utilities')            
                elem = elem.find('TargetCommonOption')
                outputname = elem.find('OutputName')
                namepos = outputname.text.find(filename)
                if namepos != 0 and outputname.text[namepos-1] != '\\':
                    f.write("[Warning] " + os.path.join(dirPath, file) + ": Output name (" + outputname.text + ") is not the same or prefixed by project name (" + filename + ").\n")
                    #outputname.text = filename
                    #file_modified = 1
                elem = elem.find('Device')
                if elem.text not in KEIL_DEVICE:
                    f.write("[Error] " + os.path.join(dirPath, file) + ": Device setting (" + elem.text + ") is not " + " or ".join(KEIL_DEVICE) + ".\n")
                elem1 = elem1.find('Cads')
                # default O2 (Optim == 3), O3 (Optim == 4)
                optim = elem1.find('Optim')
                if int(optim.text) < 3:
                    f.write("[Warning] " + os.path.join(dirPath, file) + ": C optimized level setting (" + optim.text + ") is not at least O2.\n")
                # Check optimize for time is disabled
                otime = elem1.find('oTime')
                if otime.text == '1':
                    f.write("[Warning] " + os.path.join(dirPath, file) + ": OTIME is enabled.\n")
            if file_modified:
                tree.write(os.path.join(dirPath, file), "UTF-8", True)

        for file in fnmatch.filter(fileNames, '*.uvoptx'):
            # compare project name with sample name
            filename = file.partition(".")[0]
            idx = [m.start() for m in re.finditer(r"\\", dirPath)]
            dirname = dirPath[idx[-2] + 1 : idx[-1]]
            if prj_cnt == 1 and filename != dirname:
                sys.stdout.write('.')
                sys.stdout.flush()
                f.write("[Auto-Fixed] " + os.path.join(dirPath, file) + ": uvoptx name differs from sample name (" + dirname + ").\n")
                os.rename(os.path.join(dirPath, file), os.path.join(dirPath, dirname + ".uvoptx"))
                file = dirname + ".uvoptx"
                filename = dirname
            elif prj_cnt > 1 and filename.find(dirname) != 0:
                f.write("[Warning] " + os.path.join(dirPath, file) + ": uvoptx name is not prefixed by sample name (" + dirname + ").\n")
            if debugLevel:
                print("Checking " + os.path.join(dirPath, file))
            tree = ET.parse(os.path.join(dirPath, file))
            root = tree.getroot()
            # check each target
            for elem in root.findall('Target'):
                targetname = elem.find('TargetName')
                if targetname.text.find(filename) != 0:
                    #sys.stdout.write('.')
                    #sys.stdout.flush()
                    f.write("[Warning] " + os.path.join(dirPath, file) + ": Target name (" + targetname.text + ") is not the same or prefixed by project name (" + filename + ").\n")
                    #targetname.text = filename
                    #tree.write(os.path.join(dirPath, file), "UTF-8", True)
                elem = elem.find('TargetOption')
                elem = elem.find('DebugOpt')
                elem = elem.find('pMon')
                if elem.text == None:
                    f.write("[Error] " + os.path.join(dirPath, file) + ": Download ICE setting is missing.\n")
                elif elem.text != KEIL_ICE:
                    f.write("[Error] " + os.path.join(dirPath, file) + ": Download ICE setting (" + elem.text + ") is incorrect.\n")

        # Check IAR project setting
        # Check eww, ewp, ewd project files exist
        for dir in dirNames:
            if dir.upper() == 'IAR':
                cnt = len(glob.glob(dirPath + r"\\IAR" + r"\\*.eww"))
                if cnt == 0:
                    # search shared eww file in upper folder
                    idx = [m.start() for m in re.finditer(r"\\", dirPath)]
                    dirPath2 = dirPath[0 : idx[-1]]
                    cnt += len(glob.glob(dirPath2 + r"\\*.eww"))
                if cnt == 0:
                    f.write("[Error] " + os.path.join(dirPath, 'IAR') + ": eww file is missing.\n")
                elif cnt > 1:
                    f.write("[Warning] " + os.path.join(dirPath, 'IAR') + ": More than one eww file.\n")
                prj_cnt = len(glob.glob(dirPath + r"\\IAR" + r"\\*.ewp"))
                if prj_cnt == 0:
                    f.write("[Error] " + os.path.join(dirPath, 'IAR') + ": ewp file is missing.\n")
                elif prj_cnt > 1:
                    f.write("[Warning] " + os.path.join(dirPath, 'IAR') + ": More than one ewp file.\n")
                cnt = len(glob.glob(dirPath + r"\\IAR" + r"\\*.ewd"))
                if cnt == 0:
                    f.write("[Error] " + os.path.join(dirPath, 'IAR') + ": ewd file is missing.\n")
                elif cnt > 1:
                    f.write("[Warning] " + os.path.join(dirPath, 'IAR') + ": More than one ewd file.\n")

        # Check ewp file setting
        for file in fnmatch.filter(fileNames, '*.ewp'):
            file_modified = 0
            # compare project name with directory name
            filename = file.partition(".")[0]
            idx = [m.start() for m in re.finditer(r"\\", dirPath)]
            dirname = dirPath[idx[-2] + 1 : idx[-1]]
            if prj_cnt == 1 and filename != dirname:
                #sys.stdout.write('.')
                #sys.stdout.flush()
                f.write("[Error] " + os.path.join(dirPath, file) + ": ewp name differs from sample name (" + dirname + ").\n")
                #os.rename(os.path.join(dirPath, file), os.path.join(dirPath, dirname + ".ewp"))
                #file = dirname + ".ewp"
                #filename = dirname
            elif prj_cnt > 1 and filename.find(dirname) != 0:
                f.write("[Warning] " + os.path.join(dirPath, file) + ": ewp name is not prefixed by sample name (" + dirname + ").\n")
            if debugLevel:
                print("Checking " + os.path.join(dirPath, file))
            tree = ET.parse(os.path.join(dirPath, file))
            root = tree.getroot()
            # check each configuration
            for elem in root.findall('configuration'):
                for setting in elem.findall('settings'):
                    if setting.find('name').text == "General":
                        e = setting.find('data')
                        for opt in e.findall('option'):
                            if opt.find('name').text == "OGChipSelectEditMenu":
                                if opt.find('state').text == None:
                                    f.write("[Error] " + os.path.join(dirPath, file) + ": Target device setting is missing.\n")
                                elif opt.find('state').text.find(IAR_DEVICE) != 0:
                                    f.write("[Error] " + os.path.join(dirPath, file) + ": Target device setting (" + opt.find('state').text + ") is incorrect.\n")
                            #if opt.find('name').text == "FPU":
                            #    if opt.find('state').text != "5":
                            #        f.write(file + ": FPU not enabled.\n")
                    elif setting.find('name').text == "ICCARM":
                        e = setting.find('data')
                        for opt in e.findall('option'):
                            if opt.find('name').text == "CCDiagSuppress":
                                if opt.find('state').text != None:
                                    f.write("[Warning] " + os.path.join(dirPath, file) + ": ICCARM suppress setting (" + opt.find('state').text + "), not recommended.\n")
                            # default Medium (CCOptLevel == 2), High (CCOptLevel == 3)
                            if opt.find('name').text == "CCOptLevel":
                                if int(opt.find('state').text) < 2:
                                    f.write("[Warning] " + os.path.join(dirPath, file) + ": C optimized level setting (" + opt.find('state').text + ") is not at least Medium.\n")
                    if setting.find('name').text == "ILINK":
                        e = setting.find('data')
                        for opt in e.findall('option'):
                            if opt.find('name').text == "IlinkProgramEntryLabel":
                                if opt.find('state').text != IAR_RSTHANDLER:
                                    if opt.find('state').text != None:
                                        f.write("[Warning] " + os.path.join(dirPath, file) + ": Linker entry symbol setting (" + opt.find('state').text + ") is not " + IAR_RSTHANDLER + ".\n")
                                    else:
                                        f.write("[Warning] " + os.path.join(dirPath, file) + ": Linker entry symbol setting is missing.\n")
                            if opt.find('name').text == "IlinkOverrideProgramEntryLabel":
                                if opt.find('state').text != "1":
                                    f.write("[Warning] " + os.path.join(dirPath, file) + ": Linker setting does not override default program entry.\n")
                            if opt.find('name').text == "IlinkOutputFile":
                                outname = opt.find('state').text
                                namepos = outname.find(filename)
                                if namepos != 0 and outname[namepos-1] != '\\':
                                    f.write("[Warning] " + os.path.join(dirPath, file) + ": Output name (" + opt.find('state').text + ") is not the same or prefixed by project name (" + filename + ").\n")
                                    #opt.find('state').text = outname.replace(outname, filename, 1)
                                    #file_modified = 1
                    if setting.find('name').text == "OBJCOPY":
                        e = setting.find('data')
                        for opt in e.findall('option'):
                            if opt.find('name').text == "OOCOutputFile":
                                if opt.find('state').text != None:
                                    binname = opt.find('state').text
                                    namepos = binname.find(filename)
                                    if namepos != 0 and binname[namepos-1] != '\\':
                                        f.write("[Warning] " + os.path.join(dirPath, file) + ": Output name (" + opt.find('state').text + ") is not the same or prefixed by project name (" + filename + ").\n")
                                        #opt.find('state').text = opt.find('state').text.replace(binname, filename, 1)
                                        #file_modified = 1
            if file_modified:
               tree.write(os.path.join(dirPath, file), "iso-8859-1", True)

        # Check eww file setting
        for file in fnmatch.filter(fileNames, '*.eww'):
            # compare project name with directory name
            filename = file.partition(".")[0]
            idx = [m.start() for m in re.finditer(r"\\", dirPath)]
            dirname = dirPath[idx[-2] + 1 : idx[-1]]
            if filename != dirname:
                dirname2 = dirPath[idx[-1] + 1 : ]
                if len(glob.glob(dirPath + r"\\*\\IAR")) > 0:
                    # suppose eww is shared
                    dirname = dirname2
                if filename != dirname:
                    sys.stdout.write('.')
                    sys.stdout.flush()
                    f.write("[Auto-Fixed] " + os.path.join(dirPath, file) + ": eww name differs from sample name (" + dirname + ").\n")
                    os.rename(os.path.join(dirPath, file), os.path.join(dirPath, dirname + ".eww"))
                    file = dirname + ".eww"
            if debugLevel:
                print("Checking " + os.path.join(dirPath, file))
            tree = ET.parse(os.path.join(dirPath, file))
            root = tree.getroot()
            for prj in root.findall('project'):
                prjpath = prj.find('path').text
                if prjpath.find("$WS_DIR$\\") >= 0:
                    prjpath = prjpath[len("$WS_DIR$\\"): ]
                if os.path.isfile(os.path.join(dirPath, prjpath)) == False:
                    f.write("[Error] " + os.path.join(dirPath, file) + ": Project (" + prjpath + ") not found.\n")

        # Check ewd file setting
        for file in fnmatch.filter(fileNames, '*.ewd'):
            # compare project name with directory name
            filename = file.partition(".")[0]
            idx = [m.start() for m in re.finditer(r"\\", dirPath)]
            dirname = dirPath[idx[-2] + 1 : idx[-1]]
            if prj_cnt == 1 and filename != dirname:
                sys.stdout.write('.')
                sys.stdout.flush()
                f.write("[Auto-Fixed] " + os.path.join(dirPath, file) + ": ewd name differs from sample name (" + dirname + ").\n")
                os.rename(os.path.join(dirPath, file), os.path.join(dirPath, dirname + ".ewd"))
                file = dirname + ".ewd"
                filename = dirname
            elif prj_cnt > 1 and filename.find(dirname) != 0:
                f.write("[Warning] " + os.path.join(dirPath, file) + ": ewd name is not prefixed by sample name (" + dirname + ").\n")
            if debugLevel:
                print("Checking " + os.path.join(dirPath, file))
            tree = ET.parse(os.path.join(dirPath, file))
            root = tree.getroot()
            # check each configuration
            for elem in root.findall('configuration'):
                for setting in elem.findall('settings'):
                    if setting.find('name').text == "THIRDPARTY_ID":
                        e = setting.find('data')
                        for opt in e.findall('option'):
                            if opt.find('name').text == "CThirdPartyDriverDll":
                                if opt.find('state').text.strip() != IAR_DRIVER:
                                    f.write("[Warning] " + os.path.join(dirPath, file) + ": Wrong Nu-Link driver path setting (" + opt.find('state').text + ").\n")

    f.close()
    print("Done\n")
    time.sleep(3)


# Check source code
def check_src():
    keyword_list = { "M25[1-3]", "M235[14]", "M2L31", "MAD02[56]", "M46[07]", "M48[0-7]", "MA35[DS]1", "Uart0DefaultMPF" }
    exclude_list = { "\\.git", "_CUnit", "\\Library\\CMSIS", "ThirdParty" }
    log = open('checksrc.txt', "w+")

    for dirPath, dirNames, fileNames in os.walk("."):
        isFound = False
        for exePath in exclude_list:
            if dirPath.find(exePath) >= 0:
                isFound = True
        if isFound:
            continue

        for file in fnmatch.filter(fileNames, '*.[c,h]'):
            file = dirPath + "/" + file
            file_t = open(file)
            try:
                for line in file_t:
                    # Check MFP setting
                    if line.find("_MFPH =") >= 0 and line.find("_MFPL") >= 0:
                            log.write(file + ' has wrong MFP setting:\n\t' + line)
                            sys.stdout.write('.')
                    elif line.find("_MFPL =") >= 0 and line.find("_MFPH") >= 0:
                            log.write(file + ' has wrong MFP setting:\n\t' + line)
                            sys.stdout.write('.')

                    # Check keyword list
                    for keyword in keyword_list:
                        if re.search(keyword, line):
                            log.write(file + ' found "' + keyword + '":\n\t' + line)
                            sys.stdout.write('.')

                    sys.stdout.flush()
            except:
                # Ignore codepage exception
                file_t.close()
            else:
                file_t.close()

    log.close()
    print("Done\n")
    time.sleep(3)


mymenu ="""NuMicro BSP release check script {0}
    [1] Astyle prettifier
    [2] Clean-up
    [3] Create .chm BSP manual by Doxygen
    [4] Build IAR projects
    [5] Build Keil projects
    [6] Build GCC projects
    [7] Check project setting
    [8] Check source code
    [q] Quit
"""
def show_menu():
    os.system('cls' if os.name == 'nt' else 'clear')
    print("\nWorking folder: " + os.getcwd())
    print(mymenu.format(__version__))
    
def main():
    root = os.getcwd()
    if root.find("\\_Script") > 0:
        os.chdir('..')
    elif root.find("\\bsp") < 0:
        os.chdir('bsp')

    choice = None
    while choice != 'q':
        show_menu()
        choice = input("Input your action: ")

        if choice == '1':
            astyle()
        elif choice == '2':
            cleanup()
        elif choice == '3':
            create_manual()
        elif choice == '4':
            build_iar()
        elif choice == '5':
            build_keil()
        elif choice == '6':
            build_gcc()    
        elif choice == '7':
            check_prj()
        elif choice == '8':
            check_src()
        elif choice == 'q':
            print("Bye", end="")
            for i in range(3):
                print(".", end="")
                sys.stdout.flush()
                time.sleep(0.5)
            return None


# here goes the main function
if __name__ == "__main__":
    main()
