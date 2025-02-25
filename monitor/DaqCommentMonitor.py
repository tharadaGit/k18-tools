#!/usr/bin/env python3

import os
import subprocess
from datetime import datetime
import time

#___________________________
def main():
  command = ['ls', '/misc/raid']
  subprocess.run(command)

  while True:
    cp1 = subprocess.run(
      ['tail', '-n', '200', '/misc/raid/e70_2025jan/misc/comment.txt'],
      capture_output = True,
      text = True,
    )
    cp2 = subprocess.run(
      ['grep', 'START'],
      capture_output = True,
      text = True,
      input = cp1.stdout
    )
    clear_screen()
    print(cp2.stdout)
    print(datetime.now().strftime("%Y/%m/%d %H:%M:%S")+' updated')
    time.sleep(5)

#___________________________
def clear_screen():
  os.system('cls' if os.name == 'nt' else 'clear')

#___________________________
if __name__ == "__main__":
  main()
