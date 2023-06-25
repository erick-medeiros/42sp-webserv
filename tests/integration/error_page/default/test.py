import unittest
import requests
import integration

URL = "http://localhost:8080/missing_page"

response = integration.get(URL)
expected_body = "<html><head><title>404</title><style>body{display:grid;place-content:center;margin:0;text-align:center;height:100vh}code{font-size:3em;background:#ff2a5130;border-radius:.2em}</style></head><body><code>404</code><h2>Not Found</h2></body></html>"
assert response.text == expected_body