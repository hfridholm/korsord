#
# model.py - generate crossword model using openai
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI
import argparse

api_key_file = "api.key"

#
# Parse command line arguments
#
parser = argparse.ArgumentParser(description="generate crossword model using openai")

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

args = parser.parse_args()

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

#
# Ask ChatGPT for a model
#
def model_gen(model, image_prompt):
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
                {"role": "system", "content": "You are a helpful assistant."},
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

    return model

#
# Main function
#
if __name__ == "__main__":
    if args.width < 3 or args.height < 3:
        print(f"Size too small")
        exit(1)

    model = model_create(args.width, args.height)

    if args.image:
        model = model_gen(model, args.image)

    print(model)
