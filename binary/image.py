#
# image.py - generate image using openai Dall E
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI
import requests
from PIL import Image
from io import BytesIO
import argparse
import os

api_key_file = "api.key"

#
# Function to read the API key from a local file
#
def api_key_get(filepath):
    try:
        with open(filepath, 'r') as file:
            api_key = file.read().strip()

        return api_key

    except Exception as e:
        print(f"Error reading API key: {e}")
        return None

# Start an openai client using API key
client = OpenAI(api_key=api_key_get(api_key_file))

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Image generator"
    )

    parser.add_argument("prompt",
        nargs="?",
        help="Prompt for image"
    )

    args = parser.parse_args()

    # Call the OpenAI API to generate an image
    response = client.images.generate(
      prompt=args.prompt,
      n=1,             # Number of images to generate
      size="1024x1024" # Image size, you can adjust as needed
    )

    # Get the image URL from the response
    image_url = response.data[0].url

    # Fetch the image from the URL
    image_response = requests.get(image_url)
    img = Image.open(BytesIO(image_response.content))

    img.save('image.png')
