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


def IsReported(message):
	if message.find(": error: ") >= 0:		# compile error
		return 1
	if message.find(": Error: ") >= 0:		# link error
		return 1
	if message.find(": warning: ") >= 0:
		rel_path = message[0: message.find('(')]
		abs_path = os.path.abspath(rel_path)
		for p_inc in includes:
			# Care warnings
			if abs_path.find(p_inc) >= 0 :
				for p_exc in excludes:
					# Filter
					if abs_path.find(p_exc) >= 0 :
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

	# Produce Keil project
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

	# Invoke Keil build
	os.chdir(root)					# workspace
	f = open('keil.txt', "w+")		# workspace\keil.txt

	os.chdir("bsp")					# workspace\bsp
	root = os.getcwd()

    # Clean up first
	for dirPath, dirNames, fileNames in os.walk('.'):
		for file in fnmatch.filter(fileNames, '*.o'):
			os.remove(os.path.join(dirPath, file))
	os.chdir(root)

	# Build prebuilt libraries
	for dirPath, dirNames, fileNames in os.walk('nuvoton\\libraries'):
		for file in fnmatch.filter(fileNames, '*.uvprojx'):
			print('Building', root, dirPath, file)
			os.chdir(dirPath)		# dirPath
			try:
				f1 = open(file, 'r+')
				mm = mmap.mmap(f1.fileno(), 0)
			except:
				#printf("Parse " + file + " failed\n")
				f1.close()
				pass
			else:
				# Disable cross module optimization to reduce build time
				pos = mm.find(b'<OptFeed>1</OptFeed>')
				if pos != -1:
					mm.seek(pos + 9)
					mm.write(b"0")
				mm.close()
				f1.close()
			try:
				# Keil_NVT: mdk533 with Nuvoton M0/M23 license
				#subprocess.call("C:\\Keil_NVT\\UV4\\Uv4.exe -b -j0 -z -o k.log " + file, startupinfo=si, stdout=f, stderr=f)
				# Keil_v5: mdk528a with FlexLM float license
				subprocess.call("C:\\Keil_v5\\UV4\\Uv4.exe -b -j0 -z -o k.log " + file, startupinfo=si, stdout=f, stderr=f)
			except Exception as e:
				f.write("Build " +  os.path.abspath(file) +  " has error or warning...\n")
				#print("Build" + file +  "has error or warning...\n")
				err += 1                
			except OSError:
				f.write("Ooops\n")
				pass 	#Silently ignore

			# It's a bit strange keil report error code as 0 even build failed. so parse k.log 
			tmp = open("k.log", "r")		# dirPath\k.log
			lines = tmp.readlines()
			tmp.close()
			#os.remove("k.log")
			reported = 0
			for line in lines:
				if IsReported(line):
					reported += 1
			print(reported)
			if reported > 0:
				err += 1
				f.write("Build " + os.path.abspath(file) +  " has error or warning...\n")
			os.chdir(root)			# workspace\bsp

	# Build boards
	for dirPath, dirNames, fileNames in os.walk('nuvoton'):
		if dirPath.find('\\libraries\\') >= 0:	# Skip prebuilt libraries
			continue

		for file in fnmatch.filter(fileNames, '*.uvprojx'):
			if file == "template.uvprojx":
				continue

			print('Building', root, dirPath, file)
			os.chdir(dirPath)		# dirPath
			try:
				f1 = open(file, 'r+')
				mm = mmap.mmap(f1.fileno(), 0)
			except:
				#printf("Parse " + file + " failed\n")
				f1.close()
				pass
			else:
				# Disable cross module optimization to reduce build time
				pos = mm.find(b'<OptFeed>1</OptFeed>')
				if pos != -1:
					mm.seek(pos + 9)
					mm.write(b"0")
				mm.close()
				f1.close()
			try:
				# Keil_NVT: mdk533 with Nuvoton M0/M23 license
				#subprocess.call("C:\\Keil_NVT\\UV4\\Uv4.exe -b -j0 -z -o k.log " + file, startupinfo=si, stdout=f, stderr=f)
				# Keil_v5: mdk528a with FlexLM float license
				subprocess.call("C:\\Keil_v5\\UV4\\Uv4.exe -b -j0 -z -o k.log " + file, startupinfo=si, stdout=f, stderr=f)
			except Exception as e:
				f.write("Build " +  os.path.abspath(file) +  " has error or warning...\n")
				#print("Build" + file +  "has error or warning...\n")
				err += 1                
			except OSError:
				f.write("Ooops\n")
				pass 	#Silently ignore

			# It's a bit strange keil report error code as 0 even build failed. so parse k.log 
			tmp = open("k.log", "r")	# dirPath\k.log
			lines = tmp.readlines()
			tmp.close()
			#os.remove("k.log")
			reported = 0
			for line in lines:
				if IsReported(line):
					reported += 1
			print(reported)
			if reported > 0:
				err += 1
				f.write("Build " + os.path.abspath(file) +  " has error or warning...\n")
			os.chdir(root)			# workspace\bsp
	f.close()

	# Clean up
	#rmfile_list = {"*.o", "*.lst", "*.crf", "*.i"}
	rmfile_list = {"*.o", "*.lst", "*.crf", "*.axf", "*.bin", "*.map", "*.i"}
	os.chdir(root)		# workspace\bsp
	for dirPath, dirNames, fileNames in os.walk('.'):
		for rmfile in rmfile_list:
			for file in fnmatch.filter(fileNames, rmfile):
				os.remove(os.path.join(dirPath, file))

	if err == 0:
		sys.exit(0)
	else:
		sys.exit(1)
