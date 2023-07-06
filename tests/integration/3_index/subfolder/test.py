import unittest
import requests
import integration

URL = "http://localhost:9000/path"

response = integration.get(URL)

assert response.status_code == 200
assert "index" == response.text