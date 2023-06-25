import unittest
import requests
import integration

URL_1 = "http://localhost:8080"
URL_2 = "http://localhost:8081"
URL_3 = "http://localhost:8082"

response = integration.get(URL_1)

assert response.status_code == 200

response = integration.get(URL_2)

assert response.status_code == 200

response = integration.get(URL_3)

assert response.status_code == 200
