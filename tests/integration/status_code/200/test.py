import unittest
import requests
import integration

BASE_URL = "http://localhost:8080"

response = integration.get(BASE_URL)

assert response.status_code == 200
