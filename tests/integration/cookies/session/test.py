import unittest
import requests
import integration

URL_ADMIN = "http://localhost:9000/admin"
URL_LOGIN = "http://localhost:9000/login"

VALUE = "test"

response = integration.get(URL_ADMIN)

assert response.status_code == 403

cookies = {"session": VALUE}

response = integration.get_with_cookie(URL_LOGIN, cookies)

assert response.status_code == 200

session = response.cookies.get("session")

assert session != ""

cookies = {"session": session}

response = integration.get_with_cookie(URL_ADMIN, cookies)

assert response.status_code == 200

session_value = response.headers.get("Cookie-session")

assert session_value == VALUE
