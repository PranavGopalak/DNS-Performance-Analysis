import sys
import socket
from multiprocessing import Process, Queue

def resolve_domain(queue, domain):
    try:
        ip_address = socket.gethostbyname(domain)
        queue.put(f"{domain}: {ip_address}")
    except socket.gaierror:
        queue.put(f"{domain}: Error resolving domain")

def process_file(queue, input_file):
    try:
        with open(input_file, 'r') as file:
            domains = [line.strip() for line in file if line.strip()]
        for domain in domains:
            resolve_domain(queue, domain)
    except FileNotFoundError:
        queue.put(f"Error: The file {input_file} does not exist.")
    except Exception as e:
        queue.put(f"An unexpected error occurred: {e}")

def main():
    if len(sys.argv) < 3:
        print("Usage: python main.py input_file1 [input_file2 ...] output_file")
        sys.exit(1)

    input_files = sys.argv[1:-1]
    output_file = sys.argv[-1]
    queue = Queue()
    processes = []

    for input_file in input_files:
        process = Process(target=process_file, args=(queue, input_file))
        processes.append(process)
        process.start()

    for process in processes:
        process.join()

    results = []
    while not queue.empty():
        results.append(queue.get())

    # Write all results to the output file
    try:
        with open(output_file, 'w') as file:
            for result in results:
                file.write(result + '\n')
    except IOError as e:
        print(f"Error writing to file: {e}")

if __name__ == "__main__":
    main()