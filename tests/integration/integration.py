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


text_lorem_ipsum_663b = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris ac metus consequat, consectetur justo nec, tempus justo. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Duis lobortis lorem at urna ultrices aliquam. Nulla facilisi. Maecenas accumsan venenatis mauris, id pharetra urna bibendum a. Sed id felis vitae mi consequat finibus a et odio. Morbi accumsan hendrerit velit, id sollicitudin lorem. Sed aliquet libero ut purus bibendum consequat. Sed blandit eleifend dolor, in eleifend mauris laoreet vitae. Ut gravida justo quis metus bibendum tincidunt. Nunc euismod est sed risus viverra, at cursus sapien tincidunt."

text_lorem_ipsum_1027b = (
    text_lorem_ipsum_663b
    + "Proin id tincidunt est. Phasellus cursus risus vitae lacinia suscipit. Maecenas tincidunt arcu vitae sapien fringilla lobortis. Ut feugiat ex vel mi consectetur, in ullamcorper mi bibendum. Duis vitae facilisis erat. Vestibulum ut commodo nisl. Aenean tincidunt consectetur nibh, in lobortis velit ullamcorper in. Nullam vel pellentesque magna, sed lacinia mauris."
)

text_lorem_ipsum_1732b = (
    text_lorem_ipsum_1027b
    + "Etiam auctor auctor eros, nec commodo nulla bibendum eget. Sed consequat urna vel est varius, id blandit eros ullamcorper. Aenean et justo quis erat accumsan ultricies non at purus. Sed mollis nisi eu vestibulum dapibus. Integer tincidunt purus ut arcu sollicitudin, id vulputate lectus mattis. Mauris congue odio nec justo malesuada, a fermentum erat feugiat. Quisque volutpat cursus tristique. Proin finibus elementum eros, sed suscipit neque tristique in. Morbi vitae dapibus turpis. Cras vitae est urna. Nullam accumsan ligula sit amet libero consectetur pellentesque. Sed auctor ante eu enim sollicitudin semper. Sed dapibus ipsum sit amet vestibulum cursus. Sed auctor ex at eros hendrerit faucibus."
)
