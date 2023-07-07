import unittest
import requests
import integration

URL = "http://localhost:9000/index.py"

body = "7\r\nMozilla\r\n12\r\n Developer Network\r\n0\r\n\r\n"

response = integration.post_chunked(URL, body)

assert response.status_code == 200

assert response.text == "Mozilla Developer Network" + "\n"

