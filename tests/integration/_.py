import unittest
import requests


def get(URL):
    try:
        response = requests.get(URL)
        return response
    except:
        print("Error requests.get: " + URL)
        exit(1)
