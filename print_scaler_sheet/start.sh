#!/bin/sh

work_dir=$(dirname $(readlink -f $0))

tty_file="tmp/tty.txt"

session_name=auto_scaler
session=`tmux ls | grep ${session_name}`
if [ -z "$session" ]; then
    tmux new-session -d -s ${session_name}
    tmux split-window -v -t ${session_name}
    tmux send-keys -t ${session_name}.0 "clear" C-m
    tmux send-keys -t ${session_name}.0 "tty | tee ${tty_file}" C-m
    tty=$(cat "${tty_file}")
    tmux send-keys -t ${session_name}.1 "${work_dir}/script/auto_scaler.py ${tty}" C-m
    tmux a -t ${session_name}
else
    tmux a -t ${session_name}
fi
