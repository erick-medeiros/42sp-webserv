import unittest
import requests
import _

BASE_URL = "http://localhost:8080"

response = _.get(BASE_URL)

assert response.status_code == 200
