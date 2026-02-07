# Roll No: MT25056
# Part D - Matplotlib Plots

import matplotlib.pyplot as plt
SYSTEM_INFO = "System: Ubuntu 24.04.3 LTS | Kernel 6.8.0-94 | Intel i7-8565U"



# =========================
# Data
# =========================

msg_sizes = [256, 1024, 4096, 65536]
threads = [1, 2, 4, 8]


# Throughput vs Message Size (Threads = 4)
tp_a1 = [0.000, 0.002, 0.006, 4.570]
tp_a2 = [0.340, 0.423, 1.492, 4.663]
tp_a3 = [0.291, 0.366, 1.224, 3.779]


# Latency vs Threads (Msg = 1024)
lat_a1 = [40852, 40840, 40849, 40846]
lat_a2 = [34.155, 121.716, 155.071, 169.097]
lat_a3 = [110.213, 129.073, 178.929, 209.955]


# Cache Misses vs Message Size (Threads = 4)
cache_a1 = [1366092, 1336840, 2068869, 462103193]
cache_a2 = [11483616, 8203408, 9886543, 466408290]
cache_a3 = [9284358, 9135503, 11131557, 438728816]


# Cycles (Threads = 4)
cycles_a1 = [108489649, 119973566, 129604224, 14876571499]
cycles_a2 = [46218368267, 16266859027, 16867300258, 15171652311]
cycles_a3 = [42295457430, 15711600888, 15687426797, 14502904082]

bytes_tx = [256*8, 1024*8, 4096*8, 65536*8]

cpb_a1 = [cycles_a1[i] / bytes_tx[i] for i in range(4)]
cpb_a2 = [cycles_a2[i] / bytes_tx[i] for i in range(4)]
cpb_a3 = [cycles_a3[i] / bytes_tx[i] for i in range(4)]


# =========================
# Plot 1: Throughput
# =========================

plt.figure()
plt.plot(msg_sizes, tp_a1, marker='o', label="A1 Two-Copy")
plt.plot(msg_sizes, tp_a2, marker='o', label="A2 One-Copy")
plt.plot(msg_sizes, tp_a3, marker='o', label="A3 Zero-Copy")

plt.xlabel("Message Size (Bytes)")
plt.ylabel("Throughput (Gbps)")
plt.title("Throughput vs Message Size (Threads = 4)")
plt.legend()
plt.grid()
plt.tight_layout(rect=[0, 0.12, 1, 1])
plt.figtext(0.5, 0.02, SYSTEM_INFO, ha="center", fontsize=9)


plt.savefig("plot_throughput.png")
plt.close()


# =========================
# Plot 2: Latency
# =========================

plt.figure()
plt.plot(threads, lat_a1, marker='o', label="A1 Two-Copy")
plt.plot(threads, lat_a2, marker='o', label="A2 One-Copy")
plt.plot(threads, lat_a3, marker='o', label="A3 Zero-Copy")

plt.xlabel("Threads")
plt.ylabel("Latency (Microseconds)")
plt.title("Latency vs Threads (Msg = 1024)")
plt.legend()
plt.grid()
plt.tight_layout(rect=[0, 0.12, 1, 1])
plt.figtext(0.5, 0.02, SYSTEM_INFO, ha="center", fontsize=9)



plt.savefig("plot_latency.png")
plt.close()


# =========================
# Plot 3: Cache Misses
# =========================

plt.figure()
plt.plot(msg_sizes, cache_a1, marker='o', label="A1 Two-Copy")
plt.plot(msg_sizes, cache_a2, marker='o', label="A2 One-Copy")
plt.plot(msg_sizes, cache_a3, marker='o', label="A3 Zero-Copy")

plt.xlabel("Message Size (Bytes)")
plt.ylabel("Cache Misses")
plt.title("Cache Misses vs Message Size (Threads = 4)")
plt.legend()
plt.grid()
plt.tight_layout(rect=[0, 0.12, 1, 1])
plt.figtext(0.5, 0.02, SYSTEM_INFO, ha="center", fontsize=9)



plt.savefig("plot_cache.png")
plt.close()


# =========================
# Plot 4: Cycles per Byte
# =========================

plt.figure()
plt.plot(msg_sizes, cpb_a1, marker='o', label="A1 Two-Copy")
plt.plot(msg_sizes, cpb_a2, marker='o', label="A2 One-Copy")
plt.plot(msg_sizes, cpb_a3, marker='o', label="A3 Zero-Copy")

plt.xlabel("Message Size (Bytes)")
plt.ylabel("CPU Cycles per Byte")
plt.title("Cycles per Byte vs Message Size (Threads = 4)")
plt.legend()
plt.grid()
plt.tight_layout(rect=[0, 0.12, 1, 1])
plt.figtext(0.5, 0.02, SYSTEM_INFO, ha="center", fontsize=9)



plt.savefig("plot_cycles_per_byte.png")
plt.close()


print("All plots generated successfully.")
