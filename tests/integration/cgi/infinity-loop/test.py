import unittest
import requests
import integration

URL = "http://localhost:9000/script.py"

response = integration.get(URL)

assert response.status_code == 200

assert response.text == ""
