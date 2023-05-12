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

includes = ['\\nuvoton\\', 'test\\utest\\']
excludes = ['\\packages\\', 'BMA2x2_driver', 'BMG160_driver', 'BMM050_driver']

def IsReportedWarning(message):
	if message.find(" Warning[") >= 0:
		for p_inc in includes:
			# Care warnings
			if message.find(p_inc) >= 0 :
				for p_exc in excludes:
					# Filter
					if message.find(p_exc) >= 0 :
						print('[ignore]', message)
						#Ignore
						return 0
				# Nuvoton guy
				print('[nuvoton-owned warning]', message)
				return 1
		# RTT guy
		print('[ignore]', message)
		return 0
	# Not warning
	return 0

	
if __name__ == "__main__":
	si = subprocess.STARTUPINFO()
	si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
	err = 0

	# Produce IAR project
	root = os.getcwd()								# workspace
	os.chdir("bsp\\nuvoton\\tools\\autobuild")		# workspace\bsp\nuvoton\tools\autobuild
	if os.path.isfile(os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py.weekly")):
		os.rename(os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py.weekly"), os.path.join(root, "bsp\\nuvoton\\tools\\autobuild\\auto-ci.py"))

	try:
		# Invoke env w/o blocked
		subprocess.Popen(["C:\\RTT\\env\\tools\\ConEmu\\ConEmu.exe"])
		
		# Wait produce result
		while True:
			time.sleep(3)	# 3s
			if os.path.isfile("produce.ok") == True:
				break
			if os.path.isfile("produce.failed") == True:
				err += 1
				break

		# Force kill because ConEmu cannot exit by itself
		os.system("taskkill /F /IM ConEmu.exe /T")
	except Exception as e:
		err += 1
	except OSError:
		err += 1

	# Invoke IAR build
	os.chdir(root)					# workspace
	f = open('iar2.txt', "w+")		# workspace\iar2.txt

	os.chdir("bsp")					# workspace\bsp
	root = os.getcwd()

	# Build prebuilt libraries
	for dirPath, dirNames, fileNames in os.walk('nuvoton\\libraries'):
		for file in fnmatch.filter(fileNames, '*.ewp'):
			print('Building', root, dirPath, file)
			os.chdir(dirPath)		# dirPath
			try:
				subprocess.call("C:\\Program Files (x86)\\IAR Systems\\Embedded Workbench 8.2\\common\\bin\\iarbuild.exe " + file + " -build Release -log warnings",
								startupinfo=si, stdout=f, stderr=f)
			except OSError:
				pass	#Silently ignore
			os.chdir(root)			# workspace\bsp

	# Build boards
	for dirPath, dirNames, fileNames in os.walk('nuvoton'):
		if dirPath.find('\\libraries\\') >= 0:	# Skip prebuilt libraries
			continue

		for file in fnmatch.filter(fileNames, '*.ewp'):
			if file == "template.ewp":
				continue

			print('Building', root, dirPath, file)
			os.chdir(dirPath)		# dirPath
			try:
				subprocess.call("C:\\Program Files (x86)\\IAR Systems\\Embedded Workbench 8.2\\common\\bin\\iarbuild.exe " + file + " -build Release -log warnings",
								startupinfo=si, stdout=f, stderr=f)
			except OSError:
				pass	#Silently ignore
			os.chdir(root)			# workspace\bsp
	f.close()

	# IAR output is a little messy, now clean it up
	f = open('..\\iar2.txt', "r")	# workspace\iar2.txt
	lines = f.readlines()
	f.close()
	f = open("..\\iar.txt", "w")	# workspace\iar.txt
	found = 0
	print("\n")

	for line in lines:
		if line.find("Building configuration") != -1:
			if found != 0:
				index1 = line1.find(":") + 2 # ignore ": "
				index2 = line1.find(" ", index1)
				f.write("Build " + line1[index1:index2] + " has error or warning...\n")
				print("Build " + line1[index1:index2] + " has error or warning...\n")
				err += 1
			line1 = line
			found = 0
			warning = 0
			continue
		if IsReportedWarning(line):
			warning += 1
			continue
		if line.find("Total number of errors: ") >= 0:
			if line.find("Total number of errors: 0") < 0:
				found = 1
				continue
		if line.find("Total number of warnings: ") >= 0:
			if line.find("Total number of warnings: 0") >= 0:
				continue
			if warning <= 0:
				continue
			found = 1
	f.close()

	# Clean up
	rmfile_list = {"*.o", "*.lst", "*.out"}
	os.chdir(root)		# workspace\bsp
	for dirPath, dirNames, fileNames in os.walk('.'):
		for rmfile in rmfile_list:
			for file in fnmatch.filter(fileNames, rmfile):
				os.remove(os.path.join(dirPath, file))

	if err == 0:
		sys.exit(0)
	else:
		sys.exit(1)
