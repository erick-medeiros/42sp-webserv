import unittest
import requests
import integration

URL = "http://localhost:9000/index.py"

payload = """
{
    "id": 1,
    "name": "foo"
}
"""

response = integration.post(URL, payload)

assert response.status_code == 200

assert response.text == payload + "\n"

