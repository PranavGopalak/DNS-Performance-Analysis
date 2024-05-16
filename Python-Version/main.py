import concurrent.futures
import os
import multiprocessing
import socket
import sys
import time
from pathlib import Path
import matplotlib.pyplot as plt
import pandas as pd

# Create a manager object
manager = multiprocessing.Manager()

# Create a lock
file_write_lock = manager.Lock()

def read_file(queue, input_file):
    try:
        with open(input_file, 'r') as file:
            domains = [line.strip() for line in file if line.strip()]
        for domain in domains:
            queue.put(domain)
    except FileNotFoundError:
        queue.put(f"Error: The file {input_file} does not exist.")
    except Exception as e:
        queue.put(f"An unexpected error occurred: {e}")

def resolve_and_write(queue, output_file):
    while True:
        domain = queue.get()
        if domain is None:  # No more domains will be added
            break
        try:
            ip_address = socket.gethostbyname(domain)
            result = f"{domain}: {ip_address}"
        except socket.gaierror:
            result = f"{domain}: Error resolving domain"
        # Acquire the lock before writing to the file
        with file_write_lock:
            with open(output_file, 'a') as file:
                file.write(result + '\n')
    print("Done resolving domains")

def main(consumer_count=os.cpu_count()):
    if len(sys.argv) < 3:
        print("Usage: python script.py input_file1 [input_file2 ...] output_file")
        sys.exit(1)

    input_files = sys.argv[1:-1]
    output_file = sys.argv[-1]
    domain_queue = manager.Queue()  # Use a multiprocessing Queue

    Path(output_file).unlink(missing_ok=True)

    with concurrent.futures.ProcessPoolExecutor(max_workers=len(input_files)) as executor:
        executor.map(read_file, [domain_queue]*len(input_files), input_files)

    for _ in range(os.cpu_count()):
        domain_queue.put(None)  # Signal that no more domains will be added

    with concurrent.futures.ProcessPoolExecutor(max_workers=consumer_count) as executor:
        executor.map(resolve_and_write, [domain_queue]*os.cpu_count(), [output_file]*os.cpu_count())


if __name__ == "__main__":
    # Initialize an empty DataFrame
    df = pd.DataFrame(columns=['Consumer Processes', 'Execution Time'])

    for count in [20, 14, 6, 1]:
        print(f"Running with {count} consumer processes")
        start_time = time.time()
        main(count)
        end_time = time.time()
        execution_time = end_time - start_time
        print(f"Time taken with {count} consumer processes: {execution_time} seconds\n")
        df = pd.concat([df, pd.DataFrame({'Consumer Processes': [count], 'Execution Time': [execution_time]})],
                       ignore_index=True)
    # Plot the DataFrame
    df.plot(x='Consumer Processes', y='Execution Time', kind='bar', legend=False)
    plt.ylabel('Execution Time (seconds)')
    plt.title('Execution Time by Number of Consumer Processes')
    plt.show()