import unittest
import requests
import integration

URL = "http://localhost:8080/cgi"

payload = """
{
    "id": 1,
    "name": "foo"
}
"""

response = integration.post(URL, payload)

assert response.status_code == 200

assert response.text == payload + "\n"

