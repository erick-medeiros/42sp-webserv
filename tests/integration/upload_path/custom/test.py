import unittest
import requests
import integration
import os

URL = "http://localhost:9000/uploads"
upload_path = "tmp"

# upload a file using multipart/form-data encoding with requests library
def upload_file(filename, filecontent):
    files = {"file": (filename, filecontent)}
    try:
        response = requests.post(URL, files=files)
        return response
    except:
        print("Error requests.post: " + URL)
        exit(1)

upload_file("test.txt", "hello world")

file_path = upload_path + "/test.txt"
assert os.path.exists(file_path)

file = open(file_path, "r").read()
assert file == "hello world"

# Clean up ( comment out to see the uploaded file )
os.remove(file_path)
