import unittest
import requests
import integration

URL_1 = "http://localhost:9000/index.html"
URL_2 = "http://localhost:9001/index.html"

response = integration.get(URL_1)

assert response.status_code == 200

assert "root path" in response.text

response = integration.get(URL_2)

assert response.status_code == 200

assert "root path" in response.text