import os
import sys
import time
import subprocess
import shutil
import fnmatch
import tempfile
import glob
import copy
import xml.etree.ElementTree as ET
import fileinput

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

PATH_SCRIPT = os.getcwd()
BLANK_PRJ_NAME='blank'
BLANK_EWD_PATH=os.path.join(PATH_SCRIPT , BLANK_PRJ_NAME+'.ewd')
BLANK_EWP_PATH=os.path.join(PATH_SCRIPT , BLANK_PRJ_NAME+'.ewp')

def ewp_group_copy(PRJ_EWP):
    global BLANK_EWP_PATH

    prj_ewp_tree = ET.parse(PRJ_EWP)
    prj_ewp_groups = prj_ewp_tree.findall('group')

    blank_ewp_tree = ET.parse(BLANK_EWP_PATH)
    blank_ewp_root = blank_ewp_tree.getroot()

    for group in prj_ewp_groups:
        dupe = copy.deepcopy(group)
        blank_ewp_root.append(dupe)

    blank_ewp_tree.write(PRJ_EWP, encoding='UTF-8',  xml_declaration=True)

def ewp_outfile_rename(PRJ_EWP, prjName):
    # Find Project.bin and Project.out
    with fileinput.FileInput(PRJ_EWP, inplace = True) as f:
        for line in f:
            if line.find('<state>Project.bin</state>') >= 0:
                print('                    <state>'+prjName+'.bin</state>',end ='\n')
            elif line.find('<state>Project.out</state>') >= 0:
                print('                    <state>'+prjName+'.out</state>',end ='\n')
            else:
                print(line, end ='')
        f.close()

def ewd_copy(PRJ_EWD):
    global BLANK_EWD_PATH

    blank_ewd_tree = ET.parse(BLANK_EWD_PATH)
    blank_ewd_root = blank_ewd_tree.getroot()
    blank_ewd_tree.write(PRJ_EWD, encoding='UTF-8',  xml_declaration=True)

if __name__ == "__main__":
    root = os.getcwd()
    os.chdir(root)

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):

        for file in fnmatch.filter(fileNames, '*.ewp'):

            for ip in IP_LIST:

                if file.find(ip) == 0 :

                    curName = os.path.splitext(file)[0]
                    print(dirPath + '/' +file + 'upgrading ...')
                    os.chdir(dirPath)
                    ewp_group_copy(file)
                    ewp_outfile_rename(file, curName)
                    ewd_copy(curName+'.ewd')
                    os.chdir(root)