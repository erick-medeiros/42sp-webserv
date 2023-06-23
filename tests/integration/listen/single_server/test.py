import unittest
import requests
import integration

URL = "http://localhost:8080"

response = integration.get(URL)

assert response.status_code == 200
