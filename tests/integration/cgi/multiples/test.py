import unittest
import requests
import integration

URL = "http://localhost:9000/index.py"
response = integration.get(URL)

assert response.status_code == 200
assert "hello from Python cgi\n" == response.text

URL = "http://localhost:9000/index.php"
response = integration.get(URL)

assert response.status_code == 200
assert "hello from PHP cgi\n" == response.text