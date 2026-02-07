#!/bin/bash
# Roll No: MT25056
# Part C - Automated Experiment Script (File-Based Parsing)

PORT=8080
SERVER_IP="127.0.0.1"

MSG_SIZES=(256 1024 4096 65536)
THREADS=(1 2 4 8)

CSV_FILE="MT25056_Part_C_Results.csv"
TMP_FILE="perf_tmp.txt"


# =============================
# Compile
# =============================

echo "Compiling..."

gcc MT25056_Part_A1_Server.c -o server_a1 -lpthread
gcc MT25056_Part_A1_Client.c -o client_a1 -lpthread

gcc MT25056_Part_A2_Server.c -o server_a2 -lpthread
gcc MT25056_Part_A2_Client.c -o client_a2 -lpthread

gcc MT25056_Part_A3_Server.c -o server_a3 -lpthread
gcc MT25056_Part_A3_Client.c -o client_a3 -lpthread


# =============================
# CSV Header
# =============================

echo "impl,msg_size,threads,throughput_gbps,latency_us,cycles,instructions,cache_misses,context_switches" > "$CSV_FILE"


# =============================
# Run Test
# =============================

run_test() {

    IMPL=$1
    SERVER=$2
    CLIENT=$3
    SIZE=$4
    TH=$5


    echo "Running $IMPL | Size=$SIZE | Threads=$TH"


    # Start server
    ./$SERVER $PORT $SIZE > /dev/null 2>&1 &
    SERVER_PID=$!

    sleep 2


    # Run perf and save output to file
    perf stat \
        -e cycles,instructions,cache-misses,context-switches \
        ./$CLIENT $SERVER_IP $PORT $SIZE $TH \
        > "$TMP_FILE" 2>&1


    # Kill server
    kill $SERVER_PID 2>/dev/null
    sleep 1


    # =============================
    # Parse TMP_FILE
    # =============================

    TH_LINE=$(grep "Throughput =" "$TMP_FILE" | head -1)

    THROUGHPUT=$(echo "$TH_LINE" | sed -E 's/.*Throughput = ([0-9.]+).*/\1/')
    LATENCY=$(echo "$TH_LINE" | sed -E 's/.*Latency = ([0-9.]+).*/\1/')


    CYCLES=$(grep " cycles" "$TMP_FILE" | sed -E 's/^[^0-9]*([0-9,]+).*/\1/' | tr -d ,)

    INSTR=$(grep " instructions" "$TMP_FILE" | sed -E 's/^[^0-9]*([0-9,]+).*/\1/' | tr -d ,)

    CACHEMISS=$(grep " cache-misses" "$TMP_FILE" | sed -E 's/^[^0-9]*([0-9,]+).*/\1/' | tr -d ,)

    CTX=$(grep " context-switches" "$TMP_FILE" | sed -E 's/^[^0-9]*([0-9,]+).*/\1/' | tr -d ,)


    # =============================
    # Write CSV
    # =============================

    echo "$IMPL,$SIZE,$TH,$THROUGHPUT,$LATENCY,$CYCLES,$INSTR,$CACHEMISS,$CTX" >> "$CSV_FILE"
}


# =============================
# Main Loop
# =============================

for SIZE in "${MSG_SIZES[@]}"
do
    for TH in "${THREADS[@]}"
    do
        run_test "A1" "server_a1" "client_a1" $SIZE $TH
        run_test "A2" "server_a2" "client_a2" $SIZE $TH
        run_test "A3" "server_a3" "client_a3" $SIZE $TH
    done
done


rm -f "$TMP_FILE"

echo "All experiments completed."
echo "Results saved in $CSV_FILE"
