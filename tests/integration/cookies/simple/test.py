import unittest
import requests
import integration

URL = "http://localhost:9000"

VALUE = "test"

response = integration.get(URL)

assert response.status_code == 200

cookie_session = response.cookies.get("session")

assert cookie_session == VALUE
