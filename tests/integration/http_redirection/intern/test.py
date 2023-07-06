import unittest
import requests
import integration

URL_BASE = "http://localhost:9000/"
URL_PRE_REDIR = URL_BASE + "test"
URL_POS_REDIR = URL_BASE + "foo/index.html"
STATUS_CODE_REDIRECT = 301

response = integration.get(URL_PRE_REDIR)

redirect = response.history[0]

assert redirect.url == URL_PRE_REDIR
assert redirect.status_code == STATUS_CODE_REDIRECT

assert response.url == URL_POS_REDIR
assert response.status_code == 200
assert response.text == "foo"