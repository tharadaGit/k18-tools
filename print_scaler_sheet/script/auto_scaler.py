#!/usr/bin/env python3

import os
import subprocess
import time
import signal
from datetime import datetime
import re
import argparse

## user param
season = "e70_2025jan"
spill_sec  = 4 #sec
wait_spill = 2
title_key = ["production"] #list
#############

TERMINATED = -99999

COL_RED="\033[31m"
COL_GRN="\033[32m"
COL_YLW="\033[33m"
COL_BLU="\033[34m"
COL_BYN="\033[36m"
COL_DEF="\033[0m"

SIG_={
  'W': f'{COL_YLW}WAITING{COL_DEF}',
  'R': f'{COL_GRN}RUNNING{COL_DEF}',
  'D': f'{COL_GRN}DONE{COL_DEF}',
  'T': f'{COL_RED}TERMINATED{COL_DEF}',
  'S': f'{COL_DEF}SKIPPED{COL_DEF}',
  }

#___________________________
def clear_screen():
  os.system('cls' if os.name=='nt' else 'clear')

#___________________________
def parse_line(line):
  pattern = r"(\d{4}) (\d{2}/\d{2}) (\d{2}:\d{2}:\d{2}) \[RUN (\d+)\] (.+?): (.+)"
  match = re.match(pattern, line)

  if match:
    year, date, time, runno, status, title = match.groups()
    timestamp = f"{year} {date} {time}"
    return {
      "line": line.rstrip('\n'),
      "time": timestamp,
      "runno": int(runno),
      "status": status.strip(),
      "title": title
    }
  return None

#___________________________
def main():
  last_run = None
  process = None
  with open(args.tty, "w") as pts:
    while True:
      current_time = datetime.now()
      line = subprocess.run(['tail', '-n', '1',
                             f'/misc/raid/{season}/misc/comment.txt'],
                            capture_output=True,
                            text=True).stdout
      parsed_data = parse_line(line)
      current_run = parsed_data["runno"]
      clear_screen()
      print(current_time.strftime("%Y %m/%d %H:%M:%S"))
      print(parsed_data["line"])
      print("----------------------")
      if last_run is None:
        last_run = current_run
      elif not any(word in parsed_data["title"] for word in title_key):
        formatted = ", ".join(f'"{item}"' for item in title_key)
        print(f'[RUN {current_run}] print_scaler : {SIG_["S"]} (key: {formatted})')
      elif last_run!=current_run:
        if "START" in parsed_data["status"]:
          print(f'[RUN {current_run}] print_scaler : {SIG_["W"]} "G_ON"...')
        elif "G_ON" in parsed_data["status"]:
          wait_time = wait_spill*spill_sec
          process = subprocess.Popen(f'sleep {wait_time} && print_scaler_sheet 10',
                                     shell=True, stdout=pts, stderr=pts,
                                     preexec_fn=os.setsid)
          last_run = current_run
          print(f'[RUN {current_run}] print_scaler : {SIG_["W"]} {wait_spill} spills...')
        elif last_run==TERMINATED:
          if "G_OFF" in parsed_data["status"]:
            print(f'[RUN {current_run}] print_scaler : {SIG_["T"]} -> {SIG_["W"]} "G_ON"...')
          elif "STOP" in parsed_data["status"]:
            print(f'[RUN {current_run}] print_scaler : {SIG_["T"]} -> {SIG_["W"]} "START"...')
        else:
          print(f'[RUN {current_run}] print_scaler : {SIG_["W"]} "G_ON"...')
      elif process is not None:
        if process.poll() is None:
          if "G_ON" in parsed_data["status"]:
            print(f'[RUN {current_run}] print_scaler : {SIG_["R"]} (PID={process.pid})')
          elif ("G_OFF" in parsed_data["status"]) or ("STOP" in parsed_data["status"]):
            print(f'[RUN {current_run}] print_scaler : {SIG_["T"]}')
            os.killpg(process.pid, signal.SIGKILL)
            process_terminated = subprocess.run("clear && echo 'TERMINATED'",
                                                shell=True, stdout=pts, stderr=pts)
            # process.terminate()
            last_run = TERMINATED
        else:
          process_done = subprocess.run("clear && echo 'DONE'",
                                        shell=True, stdout=pts, stderr=pts)
          print(f'[RUN {current_run}] print_scaler : {SIG_["D"]}')
      else:
        print(f'[RUN {current_run}] print_scaler : {SIG_["D"]}')
      time.sleep(1)

#___________________________
if __name__ == "__main__":
  parser = argparse.ArgumentParser(
    usage = "./auto_scaler.py <tty>",
    add_help = True
  )
  parser.add_argument("tty", type=str, help="tty for print_scaler_sheet stdout")
  args = parser.parse_args()
  main()
