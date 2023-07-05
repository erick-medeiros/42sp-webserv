import unittest
import requests
import integration

URL = "http://localhost:9000/"
URL_REDIRECT = "https://google.com"
STATUS_CODE_REDIRECT = 301

response = integration.get(URL)

redirect1 = response.history[0]
redirect2 = response.history[1]

assert redirect1.url == URL
assert redirect1.status_code == STATUS_CODE_REDIRECT
assert redirect2.url == URL_REDIRECT
assert redirect2.status_code == STATUS_CODE_REDIRECT

assert response.status_code == 200