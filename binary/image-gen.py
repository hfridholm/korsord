#
# image-gen.py - generate image using openai Dall E
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
from common import *

# Start an openai client using API key
api_key = api_key_get()

if not api_key:
    print(f"korsord: API key not found")
    sys.exit(1)

client = OpenAI(api_key=api_key)

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Generate image")

    parser.add_argument("prompt",
        type=str,
        help="Prompt for image"
    )

    parser.add_argument('--name',
        type=str, default="temp",
        help="Name of image"
    )

    args = parser.parse_args()


    print(f"Generating image")

    try:
        completion = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "Du är en expert på att skapa bildbeskrivningar för DALL-E, OpenAIs bildgenereringsmodell. När du får ett ord, begrepp eller tema, svarar du med en mycket beskrivande och kreativ prompt som kan användas direkt för att generera en bild. Svar endast med själva prompten – inga förklaringar, inga rubriker, ingen extra text." },
                {"role": "user", "content": args.prompt}
            ],
        )

        image_prompt = completion.choices[0].message.content

        print(f"{image_prompt}")

        response = client.images.generate(
          prompt=image_prompt,
          n=1,
          size="256x256"
        )

        print(f"Generated image")

        # Get the image URL from the response
        image_url = response.data[0].url

        # Fetch the image from the URL
        image_response = requests.get(image_url)
        img = Image.open(BytesIO(image_response.content))

        image_file = image_file_get(args.name)

        img.save(image_file)

        print(f"Done")

    except:
        print(f"Error: Failed to generate image")
