import unittest
import requests
import integration

URL = "http://localhost:8080/index.py"

response = integration.get(URL)

assert response.status_code == 200

# print (response.text)
# assert response.text == "hello cgi"