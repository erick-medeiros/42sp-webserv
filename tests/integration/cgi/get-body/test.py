import unittest
import requests
import integration

URL = "http://localhost:8080/cgi"

response = integration.get(URL)

assert response.status_code == 200

assert response.text == "hello cgi\n"
