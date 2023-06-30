import unittest
import requests
import integration

URL_1 = "http://localhost:9000/index.html"
URL_2 = "http://localhost:9001/index.html"
URL_3 = "http://localhost:9002/index.html"
URL_4 = "http://localhost:9003/index.html"

def check(URL):
	response = integration.get(URL)
	assert response.status_code == 200
	assert "root path" in response.text

check(URL_1)
check(URL_2)
check(URL_3)
check(URL_4)