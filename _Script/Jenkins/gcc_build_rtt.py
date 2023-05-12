import os
import sys
import time
import subprocess
import shutil
import fnmatch
import tempfile
import glob
import re
import xml.etree.ElementTree as ET	# phone home :p
import mmap

if __name__ == "__main__":
	#si = subprocess.STARTUPINFO()
	#si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
	err = 0

	root = os.getcwd()								# workspace
	os.chdir("bsp\\nuvoton\\tools\\autobuild")		# workspace\bsp\nuvoton\tools\autobuild
	if len(sys.argv) > 1:
		if os.path.isfile(os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py." + sys.argv[1])):
			os.rename(os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py." + sys.argv[1]), os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py"))
	elif os.path.isfile(os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py.gerrit")):
		os.rename(os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py.gerrit"), os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py"))
		
	try:
		# Invoke env w/o blocked
		subprocess.Popen(["C:\\RTT\\env\\tools\\ConEmu\\ConEmu.exe"])
		
		# Wait build result
		while True:
			time.sleep(3)	# 3s
			if os.path.isfile("build.ok") == True:
				break
			if os.path.isfile("build.failed") == True:
				err += 1
				break

		# Force kill because ConEmu cannot exit by itself
		os.system("taskkill /F /IM ConEmu.exe /T")

		# Clean up
		rmfile_list = {"*.o", "*.d", "*.elf", "*.hex"}
		os.chdir(root)		# workspace
		for dirPath, dirNames, fileNames in os.walk('.'):
			for rmfile in rmfile_list:
				for file in fnmatch.filter(fileNames, rmfile):
					os.remove(os.path.join(dirPath, file))

	except Exception as e:
		err += 1
	except OSError:
		err += 1

	if err == 0:
		sys.exit(0)
	else:
		sys.exit(1)
