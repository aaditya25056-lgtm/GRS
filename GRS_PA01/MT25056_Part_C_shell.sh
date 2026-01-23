#!/bin/bash

OUT="MT25056_Part_C_CSV.csv"
echo "Program+Function,CPU%,Mem(KB),IO(kB/s),Time(s)" > "$OUT"

PROGRAMS=("programA" "programB")
WORKERS=("cpu" "mem" "io")
CPU_CORE=0

killall programA programB 2>/dev/null

for P in "${PROGRAMS[@]}"; do
  for W in "${WORKERS[@]}"; do

    if [ "$P" = "programA" ]; then
       CMD="./programA $W" 
       ShortP="A"
    else
       CMD="./programB $W"
       ShortP="B"
    fi

    echo "Measuring $ShortP + $W ..."

    rm -f iostat_tmp.log
    iostat -dx 1 10 > iostat_tmp.log & 
    IO_PID=$!

    /usr/bin/time -f "%e" taskset -c $CPU_CORE $CMD >/dev/null 2> time_tmp.log &
    
    # Wait for ramp up
    sleep 1

    # --- TOP FIX: REFRESHED LOGIC ---
    TARGET_PIDS=$(pgrep -d, -x "$P")
    
    if [ -n "$TARGET_PIDS" ]; then
        # Capture 2 iterations. 
        # Awk logic: Whenever we see "PID" (header), we reset sums. 
        # This ensures we discard the first batch and only output the sums from the FINAL batch.
        STATS=$(top -b -n 2 -d 0.5 -p "$TARGET_PIDS" -w 200 | awk '
            /PID/ { cpu=0; mem=0; next } 
            { cpu+=$9; mem+=$6 } 
            END { print cpu, mem }
        ')
        CPU_TOP=$(echo "$STATS" | awk '{print $1}')
        MEM_KB=$(echo "$STATS" | awk '{print $2}')
    else
        CPU_TOP="0.0"
        MEM_KB="0"
    fi

    wait
    kill $IO_PID 2>/dev/null

    TIME_SEC=$(cat time_tmp.log)
    
    # Cleanup empty values
    if [ -z "$CPU_TOP" ]; then CPU_TOP="0.0"; fi
    if [ -z "$MEM_KB" ]; then MEM_KB="0"; fi

    IO_KB=$(awk '
        BEGIN { max=0 }
        /Device/ { for(i=1;i<=NF;i++) if($i ~ /wkB\/s|kB_wrtn\/s/) col=i }
        $1 !~ /Device|Linux|^$/ && col>0 { if ($col > max) max=$col }
        END { printf "%.2f", max }
    ' iostat_tmp.log)
    if [ -z "$IO_KB" ]; then IO_KB="0"; fi

    echo "$ShortP+$W,$CPU_TOP,$MEM_KB,$IO_KB,$TIME_SEC" >> "$OUT"
  done
done

rm -f iostat_tmp.log time_tmp.log
echo "✅ Part C completed."