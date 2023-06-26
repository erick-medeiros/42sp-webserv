import unittest
import requests
import integration

URL = "http://localhost:9000/examples"

response = integration.get(URL)

assert response.status_code == 404
