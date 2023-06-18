import os
import sys

print("Hello from Python script!")

print("SERVER_PROTOCOL: ", os.environ["SERVER_PROTOCOL"])

print("SERVER_PORT:     ", os.environ["SERVER_PORT"])

print("CONTENT_LENGTH:  ", os.environ["CONTENT_LENGTH"])

print("HTTP_HOST:       ", os.environ["HTTP_HOST"])

print("HTTP_ACCEPT:     ", os.environ["HTTP_ACCEPT"])

print("REQUEST_METHOD:  ", os.environ["REQUEST_METHOD"])

print("PATH_INFO:       ", os.environ["PATH_INFO"])

print("SCRIPT_NAME:     ", os.environ["SCRIPT_NAME"])

print("QUERY_STRING:    ", os.environ["QUERY_STRING"])
print()
print()

body = sys.argv[1]
print(body)
print("\n--------------------------------------")
print("Request body payload size: " + str(len(body)))
