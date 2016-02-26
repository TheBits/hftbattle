#!/usr/bin/env python

import shutil
import os
import sys
import json
import subprocess
import platform

script_path = os.path.dirname(os.path.realpath(__file__))
config = sys.argv[1]

executable_name = ''
system = platform.system()
if system == 'Linux':
    executable_name = 'linux_launcher'
elif system == 'Darwin':
    executable_name = 'mac_launcher'
elif system == 'Windows':
    executable_name = 'windows_launcher.exe'
else:
    print('Your OS is not supported')
    sys.exit()

executable_path = os.path.join(script_path, executable_name)
process = subprocess.Popen([executable_path, config], shell=False, stdout=subprocess.PIPE)
for line in iter(process.stdout.readline, b''):
    sys.stdout.write(line.decode())
