# Gnuplot script for Part D (Standard + Advanced Analysis)
# User: MT25056

set datafile separator ","
set term pngcairo size 1200,800 enhanced font 'Arial,10'
set grid ytics lc rgb "#bbbbbb" lw 1 lt 0
set grid xtics lc rgb "#bbbbbb" lw 1 lt 0

# Define line styles
set style line 1 lc rgb '#E41A1C' lt 1 lw 2 pt 7 ps 1.5  # Red (Process)
set style line 2 lc rgb '#377EB8' lt 1 lw 2 pt 5 ps 1.5  # Blue (Thread)
set style line 3 lc rgb '#4DAF4A' lt 1 lw 2 pt 9 ps 1.5  # Green (Ideal)

# =========================================================
# SECTION 1: STANDARD METRICS (The Basics)
# =========================================================

# 1. Execution Time (s)
set output 'MT25056_Part_D_plot_time.png'
set multiplot layout 1,3 title "Basic: Execution Time vs Concurrency" font ",14"
set xlabel "Worker Count"
set ylabel "Time (s)"

set title "CPU Intensive"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "cpu" ? $8 : 1/0) every ::1 with linespoints ls 1 title "Process (A)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "cpu" ? $8 : 1/0) every ::1 with linespoints ls 2 title "Thread (B)"

set title "Memory Intensive"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "mem" ? $8 : 1/0) every ::1 with linespoints ls 1 title "Process (A)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "mem" ? $8 : 1/0) every ::1 with linespoints ls 2 title "Thread (B)"

set title "IO Intensive"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "io" ? $8 : 1/0) every ::1 with linespoints ls 1 title "Process (A)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "io" ? $8 : 1/0) every ::1 with linespoints ls 2 title "Thread (B)"
unset multiplot

# 2. Resources
set output 'MT25056_Part_D_plot_resources.png'
set multiplot layout 1,2 title "Basic: Resource Utilization" font ",14"

set title "Memory Usage (Mem Worker)"
set ylabel "Memory (KB)"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "mem" ? $6 : 1/0) every ::1 with linespoints ls 1 title "Process (A)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "mem" ? $6 : 1/0) every ::1 with linespoints ls 2 title "Thread (B)"

set title "CPU Utilization (CPU Worker)"
set ylabel "CPU Usage (%)"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "cpu" ? $5 : 1/0) every ::1 with linespoints ls 1 title "Process (A)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "cpu" ? $5 : 1/0) every ::1 with linespoints ls 2 title "Thread (B)"
unset multiplot

# 3. IO Throughput
set output 'MT25056_Part_D_plot_io.png'
set title "Basic: Disk I/O Throughput" font ",14"
set xlabel "Worker Count"
set ylabel "Throughput (kB/s)"
set format y "%.0f"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "io" ? $7 : 1/0) every ::1 with linespoints ls 1 title "Process (A)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "io" ? $7 : 1/0) every ::1 with linespoints ls 2 title "Thread (B)"

# =========================================================
# SECTION 2: ADVANCED CORRELATIONS (The "Creative" Part)
# =========================================================

# 4. Efficiency Analysis (Time per Worker & Memory Cost)
set output 'MT25056_Part_D_plot_advanced_efficiency.png'
set multiplot layout 1,2 title "Advanced: Efficiency & Cost Analysis" font ",14"

# Graph A: Time per Worker ($8 / $4)
# Insight: Horizontal line = Perfect Scaling. Upward Slope = Context Switch Overhead.
set title "Scheduler Overhead (Time per Worker)"
set ylabel "Seconds per Worker"
set xlabel "Concurrency Count"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "cpu" ? $8/$4 : 1/0) every ::1 with linespoints ls 1 title "Process Overhead", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "cpu" ? $8/$4 : 1/0) every ::1 with linespoints ls 2 title "Thread Overhead"

# Graph B: Memory Cost per Worker ($6 / $4)
# Insight: The difference between Red/Blue is the cost of PCB vs TCB.
set title "Structure Cost (Memory per Worker)"
set ylabel "KB per Worker"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "mem" ? $6/$4 : 1/0) every ::1 with linespoints ls 1 title "Process (PCB+Heap)", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "mem" ? $6/$4 : 1/0) every ::1 with linespoints ls 2 title "Thread (Stack+Heap)"
unset multiplot

# 5. System Behavior (Fairness & IO Efficiency)
set output 'MT25056_Part_D_plot_advanced_behavior.png'
set multiplot layout 1,2 title "Advanced: System Behavior Correlations" font ",14"

# Graph C: CPU Fairness ($5 / $4)
# Insight: Visualizes the Scheduler slicing the pie. Should be 1/x curve.
set title "Scheduler Fairness (CPU Slice)"
set ylabel "CPU % Allocation"
set xlabel "Concurrency Count"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "cpu" ? $5/$4 : 1/0) every ::1 with linespoints ls 1 title "Process Slice", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "cpu" ? $5/$4 : 1/0) every ::1 with linespoints ls 2 title "Thread Slice"

# Graph D: IO Efficiency ($7 / $5) -> (Throughput / CPU)
# Insight: "How many KB do I write per 1% of CPU?" 
# If threads have lock contention, they burn more CPU for same IO, so efficiency drops.
set title "IO Efficiency (KB written per %CPU)"
set ylabel "Efficiency Index (KB/%CPU)"
plot 'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programA" && stringcolumn(3) eq "io" ? $7/$5 : 1/0) every ::1 with linespoints ls 1 title "Process Efficiency", \
     'MT25056_Part_D_CSV.csv' using 4:(stringcolumn(1) eq "programB" && stringcolumn(3) eq "io" ? $7/$5 : 1/0) every ::1 with linespoints ls 2 title "Thread Efficiency"
unset multiplot