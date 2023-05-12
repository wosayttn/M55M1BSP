import fnmatch
import os
import sys
import re
from shutil import copyfile

includes = ['*.c', '*.h'] # for files only
excludes = ['\\CMSIS\\', '\\ThirdParty\\', 'hid'] # for dirs and files

# transform glob patterns to regular expressions
global_includes = r'|'.join([fnmatch.translate(x) for x in includes])
global_excludes = r'|'.join([fnmatch.translate(x) for x in excludes]) or r'$.'

BSP_ROOT = '.'

def check_license(fname):
    pattern = ' * @copyright SPDX-License-Identifier: Apache-2.0'

    hit = 0
    f = open(fname,'r')
    for line in f.readlines():
        if (line.find(pattern) >= 0):
            hit = 1
            break
    f.close()

    if hit!=1:
        print('Can\'t found Apache2 in', fname)

def replace_license_to_apache2(fname):    
    search_pattern1 = ' * @copyright '
    search_pattern2 = ' * Copyright ('
    replaced_pattern = ' * @copyright SPDX-License-Identifier: Apache-2.0\n'

    tmp_name = fname+'.tmp'
    os.rename(fname, tmp_name) 
    
    f_old = open(tmp_name, 'r', errors='ignore')
    f_new = open(fname,'w', errors='ignore')
    for line in f_old.readlines():
        if (line.find("Nuvoton") >= 0):
            if (line.find(search_pattern1) >= 0) or (line.find(search_pattern2) >= 0):
                f_new.write(replaced_pattern)
        f_new.write(line)
    f_old.close()
    f_new.close()
    os.remove(tmp_name)

def mirror_file(src_fname, root):
    global all_file_size
    dst_fname = os.path.join('Kenny', src_fname)
    dst_folder = os.path.dirname(dst_fname)
    if not os.path.exists(dst_folder):
       os.makedirs(dst_folder)
    copyfile(src_fname, dst_fname)
    return dst_fname


projects = os.listdir(BSP_ROOT)
for item in projects:
    if item.find('Kenny') >= 0:
        continue
    bsp_path = os.path.join(BSP_ROOT, item)
    #print drivers_dir
    #print bsp_header_path
    for root, dirs, files in os.walk(bsp_path, topdown=False):
        # exclude dirs
        dirs[:] = [os.path.join(root, d) for d in dirs]
        dirs[:] = [d for d in dirs if not re.match(global_excludes, d)]

        # exclude/include files
        files = [os.path.join(root, f) for f in files]
        files = [f for f in files if not re.match(global_excludes, f)]
        files = [f for f in files if re.match(global_includes, f)]

        for fname in files:
            skip=0
            for p_exc in excludes:
                if fname.find(p_exc) >= 0:
                    skip=1
                    break

            if skip == 1:
                continue
            
            #print fname
            new_fname = mirror_file(fname, item)
            replace_license_to_apache2(new_fname)
            check_license(new_fname)
