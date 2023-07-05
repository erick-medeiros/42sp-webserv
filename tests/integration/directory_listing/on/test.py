import unittest
import requests
import integration

URL = "http://localhost:9000/examples"

response = integration.get(URL)

assert response.status_code == 200

assert "<title>Index of /examples/</title>" in response.text

assert '<a href="/examples/hello">hello</a>' in response.text

assert '<a href="/examples/world">world</a>' in response.text