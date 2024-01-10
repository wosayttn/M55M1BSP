import os
import sys
import fnmatch
import xml.etree.ElementTree as ET
import fileinput
from shutil import copyfile

sys.path.append(os.path.join(os.path.dirname(os.getcwd())))
import missudad

PROJ_FOLDER_NAME = missudad.PROJ_FOLDER_NAME
IP_LIST = missudad.IP_LIST

PATH_SCRIPT = os.getcwd()
BLANK_PRJ_NAME='blank'
BLANK_UVOPTX_PATH=os.path.join(PATH_SCRIPT , BLANK_PRJ_NAME+'.uvoptx')
BLANK_UVOPROJX_PATH=os.path.join(PATH_SCRIPT , BLANK_PRJ_NAME+'.uvprojx')

def uvprojx_group_copy(PRJ_UVOPROJX):
    global BLANK_UVOPROJX_PATH

    prj_uvprojx_tree = ET.parse(PRJ_UVOPROJX)
    prj_uvprojx_groups = prj_uvprojx_tree.find('./Targets/Target/Groups')

    blank_uvprojx_tree = ET.parse(BLANK_UVOPROJX_PATH)
    blank_uvprojx_target = blank_uvprojx_tree.find('./Targets/Target')

    blank_uvprojx_target.append(prj_uvprojx_groups)

    blank_TargetArmAds = blank_uvprojx_tree.find('./Targets/Target/TargetOption/TargetArmAds')
    blank_TargetArmAds.remove(blank_TargetArmAds.find('Cads'))
    blank_TargetArmAds.remove(blank_TargetArmAds.find('Aads'))
    blank_TargetArmAds.remove(blank_TargetArmAds.find('LDads'))

    prj_TargetArmAds = prj_uvprojx_tree.find('./Targets/Target/TargetOption/TargetArmAds')
    blank_TargetArmAds.append(prj_TargetArmAds.find('Cads'))
    blank_TargetArmAds.append(prj_TargetArmAds.find('Aads'))
    blank_TargetArmAds.append(prj_TargetArmAds.find('LDads'))

    blank_uvprojx_tree.write(PRJ_UVOPROJX, encoding='UTF-8',  xml_declaration=True, default_namespace=None)

def uvprojx_outfile_rename(PRJ_UVOPROJX, prjName):
    # Find Project.bin and Project.out
    with fileinput.FileInput(PRJ_UVOPROJX, inplace = True) as f:
        for line in f:
            if line.find('<TargetName>Project</TargetName>') >= 0:
                print('      <TargetName>'+prjName+'</TargetName>',end ='\n')
            elif line.find('<OutputName>Project</OutputName>') >= 0:
                print('          <OutputName>'+prjName+'</OutputName>',end ='\n')
            elif line.find('<?xml version=\'1.0\' encoding=\'UTF-8\'?>') >= 0:
                print('<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>',end ='\n')
            else:
                print(line, end ='')
        f.close()

def uvoptx_group_copy(PRJ_UVOPTX):
    global BLANK_UVOPTX_PATH

    prj_uvoptx_tree = ET.parse(PRJ_UVOPTX)
    prj_uvoptx_groups = prj_uvoptx_tree.findall('Group')

    blank_uvoptx_tree = ET.parse(BLANK_UVOPTX_PATH)
    blank_uvoptx_root = blank_uvoptx_tree.getroot()

    for group in prj_uvoptx_groups:
        #dupe = copy.deepcopy(group)
        blank_uvoptx_root.append(group)

    blank_uvoptx_tree.write(PRJ_UVOPTX, encoding='UTF-8',  xml_declaration=True, default_namespace=None)

def uvoptx_outfile_rename(PRJ_UVOPTX, prjName):
    # Find Project.bin and Project.out
    with fileinput.FileInput(PRJ_UVOPTX, inplace = True) as f:
        for line in f:
            if line.find('<TargetName>Project</TargetName>') >= 0:
                print('    <TargetName>'+prjName+'</TargetName>',end ='\n')
            elif line.find('<OutputName>Project</OutputName>') >= 0:
                print('    <OutputName>'+prjName+'</OutputName>',end ='\n')
            elif line.find('<?xml version=\'1.0\' encoding=\'UTF-8\'?>') >= 0:
                print('<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>',end ='\n')
            else:
                print(line, end ='')
        f.close()

def uvoptx_copy(PRJ_EWD):
    global BLANK_UVOPTX_PATH

    blank_uvoptx_tree = ET.parse(BLANK_UVOPTX_PATH)
    blank_uvoptx_tree.write(PRJ_EWD, encoding='UTF-8',  xml_declaration=True, default_namespace=None)

if __name__ == "__main__":
    root = os.getcwd()
    os.chdir(root)

    for dirPath, dirNames, fileNames in os.walk(PROJ_FOLDER_NAME):

        for file in fnmatch.filter(fileNames, '*.uvprojx'):

            for ip in IP_LIST:

                if file.find(ip) == 0 :

                    curName = os.path.splitext(file)[0]
                    print(dirPath + '/' + file + ' upgrading ...')
                    os.chdir(dirPath)

                    uvprojx_group_copy(file)
                    uvprojx_outfile_rename(file, curName)

                    #uvoptx_copy(curName+'.uvoptx')
                    uvoptx_group_copy(curName+'.uvoptx')
                    uvoptx_outfile_rename(curName+'.uvoptx', curName)

                    copyfile( os.path.join(root, 'Nu_Link_Driver.ini'), os.path.join(os.getcwd(), 'Nu_Link_Driver.ini') )

                    os.chdir(root)
