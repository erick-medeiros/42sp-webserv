import unittest
import requests
import integration

URL = "http://localhost:9000/index.py"

response = integration.get(URL)
assert response.status_code == 200
assert 'print("hello cgi")' in response.text
