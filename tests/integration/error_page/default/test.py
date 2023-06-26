import unittest
import requests
import integration

URL = "http://localhost:9000/missing_page"

response = integration.get(URL)

assert response.status_code == 404
assert "<title>404</title>" in response.text
assert "Not Found" in response.text