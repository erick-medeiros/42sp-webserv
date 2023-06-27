import unittest
import requests
import integration

URL = "http://localhost:9000/index.html"

response = integration.get(URL)

assert response.status_code == 200
assert "<title>Example</title>" in response.text
assert "Hello World" in response.text