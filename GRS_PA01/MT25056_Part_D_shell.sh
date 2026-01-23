#!/bin/bash

OUT="MT25056_Part_D_CSV.csv"
echo "Program,Type,Worker,Count,CPU%,Mem(KB),IO(kB/s),Time(s)" > "$OUT"

CPU_CORE=0
WORKERS=("cpu" "mem" "io")
PROC_COUNTS=(2 3 4 5)
THREAD_COUNTS=(2 3 4 5 6 7 8)

killall programA programB 2>/dev/null

CURRENT=0
TOTAL=33

measure() {
    PROG=$1
    TYPE=$2
    W=$3
    N=$4
    CMD="./$PROG $W $N"
    
    CURRENT=$((CURRENT+1))
    echo "[$CURRENT/$TOTAL] Measuring $PROG ($W x $N)..."

    rm -f iostat_temp.log
    iostat -dx 1 10 > iostat_temp.log & 
    IO_PID=$!

    /usr/bin/time -f "%e" taskset -c $CPU_CORE $CMD >/dev/null 2> time_temp.log &
    
    sleep 1

    TARGET_PIDS=$(pgrep -d, -x "$PROG")
    if [ -n "$TARGET_PIDS" ]; then
        # RESET logic: Clears sum on every header, keeps only the last block.
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

    TIME_SEC=$(cat time_temp.log)

    if [ -z "$CPU_TOP" ]; then CPU_TOP="0.0"; fi
    if [ -z "$MEM_KB" ]; then MEM_KB="0"; fi

    IO_KB=$(awk '
        BEGIN { max=0 }
        /Device/ { for(i=1;i<=NF;i++) if($i ~ /wkB\/s|kB_wrtn\/s/) col=i }
        $1 !~ /Device|Linux|^$/ && col>0 { if ($col > max) max=$col }
        END { printf "%.2f", max }
    ' iostat_temp.log)
    if [ -z "$IO_KB" ]; then IO_KB="0"; fi

    echo "$PROG,$TYPE,$W,$N,$CPU_TOP,$MEM_KB,$IO_KB,$TIME_SEC" >> "$OUT"
}

for W in "${WORKERS[@]}"; do
  for N in "${PROC_COUNTS[@]}"; do
    measure "programA" "process" "$W" "$N"
  done
done

for W in "${WORKERS[@]}"; do
  for N in "${THREAD_COUNTS[@]}"; do
    measure "programB" "thread" "$W" "$N"
  done
done

rm -f iostat_temp.log time_temp.log
echo "Part D completed."

if command -v gnuplot &> /dev/null; then
    gnuplot MT25056_Part_D_plot_results.gp
    echo "Plots generated."
fi