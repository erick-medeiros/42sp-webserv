import unittest
import requests
import integration

URL = "http://localhost:8080/missing_page"
error_page = "error_pages/404.html"

response = integration.get(URL)
expected_body = open(error_page).read()
assert response.text == expected_body