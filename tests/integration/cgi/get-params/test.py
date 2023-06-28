import unittest
import requests
import integration

params = "arg1=123&arg2=456"

URL = "http://localhost:9000/index.py?" + params

response = integration.get(URL)

assert response.status_code == 200

assert response.text == params + "\n"
