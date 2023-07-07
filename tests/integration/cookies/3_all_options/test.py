import unittest
import requests
import integration

URL = "http://localhost:9000"

NAME = "session"
VALUE = "test"

response = integration.get(URL)

assert response.status_code == 200

assert response.cookies.get(NAME) == VALUE

for cookie in response.cookies:
	if (cookie.name == NAME):
		# secure
		assert cookie.secure
		# httponly
		try:
			cookie._rest['httponly']
		except:
			print("no httponly")
			exit(1)
		# samesite
		try:
			assert cookie._rest['samesite'] == "lax"
		except:
			print("no samesite")
			exit(1)