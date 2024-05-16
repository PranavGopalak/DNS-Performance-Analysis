# DNS Resolver Performance Analysis

This project aims to compare the performance of DNS resolvers implemented in C and Python. By implementing DNS resolvers in both languages, we can evaluate their respective performance characteristics and determine which language is better suited for DNS resolution tasks.

## C DNS Resolver

The C DNS resolver is implemented using low-level socket programming and the standard C library functions. It leverages the efficiency and speed of C to perform DNS resolution tasks. The resolver is designed to handle multiple concurrent DNS queries efficiently, making it suitable for high-performance applications.

## Python DNS Resolver

The Python DNS resolver is implemented using the `socket` module and other relevant libraries. It provides a higher-level abstraction compared to the C implementation, making it easier to write and understand. However, due to the nature of Python being an interpreted language, it may not be as performant as the C implementation for high-throughput scenarios.

## Performance Metrics

To compare the performance of the DNS resolvers, we will measure the following metrics:

1. **Response Time**: The time taken by each resolver to receive a response from the DNS server.
2. **Throughput**: The number of DNS queries that can be processed per unit of time by each resolver.
3. **Memory Usage**: The amount of memory consumed by each resolver during DNS resolution tasks.

## Getting Started

To run the performance analysis, follow these steps:

1. Clone this repository to your local machine.
2. Navigate to the `C-Version` directory for the C implementation or the `Python-Version` directory for the Python implementation.
3. Follow the instructions in the respective directories to build and run the DNS resolver.
4. Use the provided test cases or create your own to evaluate the performance of each resolver.