import unittest
import requests


def get(URL):
    try:
        response = requests.get(URL)
        return response
    except:
        print("Error requests.get: " + URL)
        exit(1)

def post(URL, body):
    try:
        response = requests.post(URL, data=body)
        return response
    except:
        print("Error requests.post: " + URL)
        exit(1)
