import sys
import time
import requests
from concurrent.futures import ThreadPoolExecutor

def make_request(url, request_number):
    start_time = time.time()
    response = requests.get(url)
    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"Request #{request_number}: {url} took {elapsed_time:.2f} seconds")

# Check if the port argument is provided
if len(sys.argv) < 2:
    print("Please provide the port number as an argument.")
    sys.exit(1)

# Retrieve the port number from the command-line argument
port = sys.argv[1]

# Specify the URL of your web server with the provided port
url = f"http://localhost:{port}"

# Number of concurrent requests
num_requests = 10

# Create a ThreadPoolExecutor to manage the concurrent requests
with ThreadPoolExecutor(max_workers=num_requests) as executor:
    # Submit the slow requests to the executor
    futures = [executor.submit(make_request, url, i+1) for i in range(num_requests)]

    # Wait for all requests to complete
    for future in futures:
        future.result()
