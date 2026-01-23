# GRS Assignment 1 - Process vs Thread Performance Analysis

**Roll No:** MT25056
**Name:** Aaditya Shinde

## Project Overview
This project compares the performance characteristics of **Processes (fork)** versus **Threads (pthread)** under three distinct workloads:
1.  **CPU Intensive:** Mathematical calculations (Square root approximation loop).
2.  **Memory Intensive:** Large array allocation (10MB) and access patterns.
3.  **I/O Intensive:** Heavy file writing operations.

The project analyzes execution time, CPU utilization, Memory footprint, and Disk I/O throughput to demonstrate the overhead differences between context switching processes versus threads.

## File Structure

### Source Code
* **`MT25056_Part_A_Program_A.c`**: Main driver code for Process-based parallelism.
* **`MT25056_Part_A_Program_B.c`**: Main driver code for Thread-based parallelism.
* **`MT25056_Part_B_workers.c`**: Shared implementation of CPU, Memory, and I/O tasks.
* **`MT25056_Part_B_workers.h`**: Header file defining worker interfaces.

### Automation Scripts
* **`MT25056_Part_C_shell.sh`**: Bash script for Part C (Single-instance benchmarking).
* **`MT25056_Part_D_shell.sh`**: Bash script for Part D (Scaling analysis 2-8 workers).
* **`MT25056_Part_D_plot_results.gp`**: Gnuplot script to generate performance graphs from CSV data.

### Build Config
* **`Makefile`**: Automation for compiling, cleaning, and running the experiments.

---

## Prerequisites
To run this project, the following tools must be installed in the Linux environment:
* `gcc` (GNU Compiler Collection)
* `make` (Build automation)
* `sysstat` (Required for `iostat` command)
* `gnuplot` (Required for generating graphs)

## Compilation Instructions

1.  **Build the Project:**
    Compiles the source code and generates executables `programA` and `programB`.
    ```bash
    make
    ```

2.  **Clean Build Files:**
    Removes object files, executables, logs, and generated CSVs/Plots.
    ```bash
    make clean
    ```

---

## Execution Instructions

### Part C: Single-Task Analysis
Runs both programs with 1 worker for all task types (CPU, Mem, IO) and records the overhead.

**Command:**
```bash
make run_partC
```
* **Output:** Generates `MT25056_Part_C_CSV.csv`.

### Part D: Scaling & Concurrency Analysis
Runs the programs with increasing worker counts (2 to 8), captures metrics, and automatically generates plots.

**Command:**
```bash
make run_partD
```
* **Output:**
    1.  **Data:** `MT25056_Part_D_CSV.csv`
    2.  **Standard Plots:**
        * `MT25056_Part_D_plot_time.png` (Execution Time vs Concurrency)
        * `MT25056_Part_D_plot_resources.png` (CPU & Memory Usage)
        * `MT25056_Part_D_plot_io.png` (Disk Throughput)
    3.  **Advanced Analysis Plots:**
        * `MT25056_Part_D_plot_advanced_efficiency.png` (Scheduler Overhead & Structure Cost)
        * `MT25056_Part_D_plot_advanced_behavior.png` (Fairness & IO Efficiency)
