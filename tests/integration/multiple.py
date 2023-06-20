import unittest
import requests
import _

BASE_URL_1 = "http://localhost:8080"
BASE_URL_2 = "http://localhost:9000"

response = _.get(BASE_URL_1)

assert response.status_code == 200

response = _.get(BASE_URL_2)

assert response.status_code == 200
