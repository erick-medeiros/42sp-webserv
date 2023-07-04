import unittest
import requests
import integration

URL = "http://localhost:9000/"
response = integration.get(URL)
assert response.status_code == 501
