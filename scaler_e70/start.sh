#!/bin/sh

season=2025jan

#_______________________________________________________________________________
top_dir=$(dirname $(readlink -f $0))

name=IntegratedKaon_e70_${season}
session_name=${name}
pdf=${name}.pdf
session=`tmux ls | grep ${session_name}`
if [ -z "$session" ]; then
    echo "create new session ${session_name}"
    tmux new-session -d -s ${session_name}
    tmux split-window -v -t ${session_name}
    tmux send-keys -t ${session_name}.0 "cd ${top_dir}/macro" C-m
    tmux send-keys -t ${session_name}.0 "watch ${top_dir}/macro/IntegratedKaon $season" C-m
    tmux send-keys -t ${session_name}.1 "evince ${top_dir}/pdf/${pdf} &" C-m
    tmux a -t ${session_name}
else
    echo "reattach session ${session_name}"
    tmux a -t ${session_name}
fi
