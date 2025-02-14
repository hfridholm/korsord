#
# model-gen.py - generate crossword model using openai
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI
import argparse
import os
import subprocess
from common import *

api_key_file = "api.key"

# Start an openai client using API key
client = OpenAI(api_key=api_key_get(api_key_file))

#
# Ask ChatGPT for a model
#
def model_image_gen(model, image_prompt):
    prompt = f"""
Jag vill att du lägger till en bild i följande rutnät:
{model}

Byt ut '.' mot 'X' där bilden ska vara. Det är livsviktigt att bilden ska vara rektangulär och får inte vara mer än hälftan av varken höjden eller bredden.

Följande mening berättar vart bilden ska vara:
"{image_prompt}"

Om, och endast om, bilden ska vara i ett hörn måste den vara längst in i hörnet.

Det är livsviktigt att du endast svarar med det ändrade rutnätet, ingen annan text.
    """

    print(f"Prompt:\n{prompt}\n")

    try:
        completion = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "Du är en logisk assistent"},
                {"role": "user", "content": prompt}
            ],
            temperature=0.7
        )

        message = completion.choices[0].message.content

        print(f"Response:\n{message}\n")

        return message

    except Exception as exception:
        print(f"ChatGPT error: {exception}")
        return None

#
#
#
def model_create(width, height):
    model = ""

    for y in range(height):
        for x in range(width):
            model += ". "

        model += "\n"

    return model.strip('\n')

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Generate crossword model using openai")

    parser.add_argument("width",
        type=int,
        help="Width of grid"
    )

    parser.add_argument("height",
        type=int,
        help="Height of grid"
    )

    parser.add_argument("--image",
        type=str, default=None,
        help="Position and size of image"
    )

    parser.add_argument("--name",
        type=str, default="temp",
        help="Name of grid"
    )

    parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing model"
    )

    args = parser.parse_args()


    # Disregard too small size
    if args.width < 3 or args.height < 3:
        print(f"korsord: {args.width}x{args.height}: Size too small")
        exit(0)


    model_file = model_file_get(args.name)

    if os.path.exists(model_file) and not args.force:
        if args.name == "temp":
            print(f"korsord: Model already exists")

        else:
            print(f"korsord: {args.name}: Model already exists")

        exit(0)


    model = model_create(args.width, args.height)

    if args.image:
        model = model_image_gen(model, args.image)

    subprocess.run(f'echo "{model}" > {model_file}', shell=True, check=True)

    if args.name == "temp":
        print(f"Generated model")

    else:
        print(f"Generated model named '{args.name}'")
