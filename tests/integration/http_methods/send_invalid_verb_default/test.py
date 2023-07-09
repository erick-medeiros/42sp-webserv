import unittest
import requests
import integration

URL = "http://localhost:9000/"

# sending GET instead of POST expects NOT_IMPLEMENTED status code response
response = integration.get(URL)

assert response.status_code == 405
