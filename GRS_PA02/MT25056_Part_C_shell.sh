#!/bin/bash
# Roll No: MT25056
# Part C: Final Assignment Script (Hardware Metrics Version)

# --- STEP 0: FIND PERF (Auto-detect) ---
# Try to find the perf binary that matches the running kernel
PERF_CMD="/usr/lib/linux-tools/$(uname -r)/perf"
if [ ! -f "$PERF_CMD" ]; then
    # If exact match fails, try the one we found earlier (6.8)
    PERF_CMD="/usr/lib/linux-tools/6.8.0-94-generic/perf"
fi
if [ ! -f "$PERF_CMD" ]; then
    # Last resort: global command
    PERF_CMD="perf"
fi
echo "Using Perf: $PERF_CMD"

# --- STEP 1: NETWORK SETUP ---
echo "Initializing network namespaces..."
sudo ip netns del server_ns 2>/dev/null
sudo ip netns del client_ns 2>/dev/null
sudo ip netns add server_ns
sudo ip netns add client_ns
sudo ip link add veth0 type veth peer name veth1
sudo ip link set veth0 netns server_ns
sudo ip link set veth1 netns client_ns
sudo ip netns exec server_ns ip addr add 10.0.0.1/24 dev veth0
sudo ip netns exec server_ns ip link set veth0 up
sudo ip netns exec server_ns ip link set lo up
sudo ip netns exec client_ns ip addr add 10.0.0.2/24 dev veth1
sudo ip netns exec client_ns ip link set veth1 up
sudo ip netns exec client_ns ip link set lo up
echo "Network ready."

# --- STEP 2: CONFIGURATION ---
MSIZES=(1024 4096 16384 65536)
THREADS=(1 2 4 8)
DURATION=10
CSV_FILE="MT25056_Part_C_CSV.csv"

# Start fresh
rm -f "$CSV_FILE"
# METRICS RESTORED TO ASSIGNMENT REQUIREMENTS:
echo "Implementation,MsgSize,Threads,Throughput_Gbps,Latency_us,Cycles,Cache_Misses,Context_Switches" > "$CSV_FILE"

run_benchmark() {
    LABEL=$1; SRV_EXE=$2; CLI_EXE=$3
    for sz in "${MSIZES[@]}"; do
        for t in "${THREADS[@]}"; do
            echo "------------------------------------------------"
            echo "Profiling $LABEL: Size=$sz, Threads=$t"
            
            # 1. NUKE ZOMBIES
            sudo killall -9 $SRV_EXE 2>/dev/null
            
            # 2. Start Server
            sudo ip netns exec server_ns ./$SRV_EXE &
            sleep 2 
            
            # 3. GET EXACT PID
            REAL_PID=$(pgrep -n $SRV_EXE)
            if [ -z "$REAL_PID" ]; then
                echo "ERROR: Server did not start!"
                continue
            fi

            # 4. Capture App metrics
            CLI_OUT=$(sudo ip netns exec client_ns ./$CLI_EXE $sz $t $DURATION 2>/dev/null)
            THR=$(echo "$CLI_OUT" | grep "Throughput" | awk '{print $2}')
            LAT=$(echo "$CLI_OUT" | grep "Avg Latency" | awk '{print $3}')

            # 5. Capture HARDWARE metrics (Cycles/Cache-Misses)
            # We add 2>/dev/null to suppress kernel mismatch warnings
            sudo ip netns exec server_ns $PERF_CMD stat -e cycles,cache-misses,context-switches -x, -o perf_temp.txt -p $REAL_PID -- sleep 5 2>/dev/null
            
            if [ ! -f perf_temp.txt ]; then
                echo "   !!! PERF FAILED (File not created) !!!"
                CYC="ERR"; CMS="ERR"; CSW="ERR"
            else
                CYC=$(grep "cycles" perf_temp.txt | cut -d, -f1)
                CMS=$(grep "cache-misses" perf_temp.txt | cut -d, -f1)
                CSW=$(grep "context-switches" perf_temp.txt | cut -d, -f1)
            fi

            echo "   -> Stats: Thr=$THR, Lat=$LAT, Cyc=$CYC"
            echo "$LABEL,$sz,$t,$THR,$LAT,$CYC,$CMS,$CSW" >> "$CSV_FILE"
            
            # Cleanup
            sudo kill -9 $REAL_PID 2>/dev/null
            wait $REAL_PID 2>/dev/null
            rm -f perf_temp.txt
            sleep 1
        done
    done
}

# --- STEP 3: EXECUTE ---
run_benchmark "Two-Copy" "server_a1" "client_a1"
run_benchmark "One-Copy" "server_a2" "client_a2"
run_benchmark "Zero-Copy" "server_a3" "client_a3"

echo "------------------------------------------------"
echo "All experiments complete. Results saved in $CSV_FILE"