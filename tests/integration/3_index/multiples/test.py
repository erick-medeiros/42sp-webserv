import unittest
import requests
import integration

URL_1 = "http://localhost:9000"
URL_2 = "http://localhost:9001"
URL_3 = "http://localhost:9002"
URL_4 = "http://localhost:9003"

response = integration.get(URL_1)
assert response.status_code == 200
assert "one" in response.text

response = integration.get(URL_2)
assert response.status_code == 200
assert "two" in response.text

response = integration.get(URL_3)
assert response.status_code == 200
assert "three" in response.text

response = integration.get(URL_4)
assert response.status_code == 200
assert "one" in response.text