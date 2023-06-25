import unittest
import requests
import integration

URL = "http://localhost:8080"

# 1024 * 1024 = 1048576 (1M em bytes)

larg_text = integration.text_lorem_ipsum_1027b * 1021

assert len(larg_text) < (1024 * 1024)

response = integration.post(URL, larg_text)

assert response.status_code == 200