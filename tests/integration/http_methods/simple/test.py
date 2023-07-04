import unittest
import requests
import integration

URL = "http://localhost:9000/"
response = integration.get(URL)
assert response.status_code == 200
assert "Foo" in response.text

URL = "http://localhost:9000/"
response = integration.post(URL, "payload")
assert response.status_code == 200
assert "Foo" in response.text

URL = "http://localhost:9000/"
response = integration.delete(URL)
assert response.status_code == 200
assert "Foo" in response.text
