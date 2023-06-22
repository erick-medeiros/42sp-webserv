import unittest
import requests
import integration

DEFAULT_URL = "http://localhost:8080"
LIMITED_URL = "http://localhost:8081"

body_619b = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc quis erat eget augue dictum sagittis ut ac quam. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Etiam dignissim lobortis odio, vitae suscipit neque convallis ut. Cras feugiat ex at lorem finibus, hendrerit sagittis lectus imperdiet. Nullam rhoncus risus a porta gravida. Vestibulum porta dolor a urna vestibulum scelerisque. Cras aliquet, mi nec posuere imperdiet, libero purus ultricies nunc, at iaculis enim orci sit amet lacus. Suspendisse accumsan vel orci in dapibus. Aliquam condimentum ex ac imperdiet pretium."

response = integration.post(DEFAULT_URL, body_619b)
assert response.status_code == 200

body_1238b = body_619b + body_619b

response = integration.post(LIMITED_URL, body_1238b)

assert response.status_code == 413

