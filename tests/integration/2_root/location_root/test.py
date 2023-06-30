import unittest
import requests
import integration

URL_1 = "http://localhost:9000/"
URL_2 = "http://localhost:9001/"
URL_3 = "http://localhost:9002/"
URL_4 = "http://localhost:9003/"

def check(URL):
	response = integration.get(URL + "index.html")
	assert response.status_code == 200
	assert "main" in response.text
	response = integration.get(URL + "location/index.html")
	# assert response.status_code == 200
	# assert "location" in response.text

check(URL_1)
check(URL_2)
check(URL_3)
check(URL_4)