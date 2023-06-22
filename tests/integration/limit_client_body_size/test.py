import unittest
import requests
import integration

BASE_URL = "http://localhost:8080"
DEFAULT_URL = BASE_URL + "8080/"
URL_WITH_LIMIT = BASE_URL + "8081/"

body = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc quis erat eget augue dictum sagittis ut ac quam. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Etiam dignissim lobortis odio, vitae suscipit neque convallis ut. Cras feugiat ex at lorem finibus, hendrerit sagittis lectus imperdiet. Nullam rhoncus risus a porta gravida. Vestibulum porta dolor a urna vestibulum scelerisque. Cras aliquet, mi nec posuere imperdiet, libero purus ultricies nunc, at iaculis enim orci sit amet lacus. Suspendisse accumsan vel orci in dapibus. Aliquam condimentum ex ac imperdiet pretium."

response = integration.post(BASE_URL, body)
assert response.status_code == 200

# response = integration.post(URL_WITH_LIMIT, body)
# assert response.status_code == 413

