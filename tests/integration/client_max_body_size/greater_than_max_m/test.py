import unittest
import requests
import integration

URL = "http://localhost:8080"

response = integration.post(URL, 1024 * integration.text_lorem_ipsum_1027b)

assert response.status_code == 413
