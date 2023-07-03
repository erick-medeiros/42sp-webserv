import unittest
import requests
import integration


URL = "http://localhost:9000/"
response = integration.post(URL, "payload")
assert response.status_code == 200
assert "Foo" in response.text

# sending POST where we only want to accept GET and DELETE
URL = "http://localhost:9000/bar"
response = integration.post(URL, "payload")
assert response.status_code == 501