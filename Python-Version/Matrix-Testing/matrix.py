from math import floor

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import time
import multiprocessing

def worker(args):
    A, B = args
    return np.dot(A, B)

def matrix_multiplication(n, num_processes, num_multiplications):
    # Create two random n x n matrices
    A = np.random.rand(n, n)
    B = np.random.rand(n, n)

    # Record the start time
    start_time = time.time()

    # Create a Pool of processes
    with multiprocessing.Pool(processes=num_processes) as pool:
        # Perform matrix multiplication

        for _ in range(floor(num_multiplications/num_processes)):
            C = np.array(pool.map(worker, [(A[i], B) for i in range(n)]))

    # Record the end time
    end_time = time.time()

    # Calculate and return the time taken
    return n, end_time - start_time


if __name__ == "__main__":
    # Initialize an empty DataFrame
    df = pd.DataFrame(columns=['Matrix Size', 'Execution Time'])

    # Test for different matrix sizes
    for n in [500, 1000, 2500]:
        for num_processes in [1,6,14,20]:
            matrix_size, time_taken = matrix_multiplication(n, num_processes, 1000)
            print(f"Time taken for {n}x{n} matrix multiplication with {num_processes} processes: {time_taken} seconds")
            # df = pd.concat([df, pd.DataFrame({'Consumer Processes': [count], 'Execution Time': [execution_time]})],
            #                ignore_index=True)
            # Append the results to the DataFrame
            df = pd.concat([df, pd.DataFrame({'Matrix Size': [matrix_size], 'Execution Time': [time_taken],
                                             'Processes': [num_processes]})], ignore_index=True)
    # Plot the DataFrame
    df.pivot(index='Matrix Size', columns='Processes', values='Execution Time').plot(kind='line')
    plt.ylabel('Execution Time (seconds)')
    plt.title('Execution Time by Matrix Size and Number of Processes')
    plt.show()