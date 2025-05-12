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
import numpy
from common import *

# Start an openai client using API key
api_key = api_key_get()

if not api_key:
    print(f"korsord: API key not found")
    sys.exit(1)

client = OpenAI(api_key=api_key)

#
# Extract encoded image position and size using ChatGPT
#
def model_image_values_get(image_prompt):
    prompt_values = {
        "{image}": image_prompt
    }

    prompt = prompt_load("model", prompt_values)

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

        lines = message.split("\n")

        if len(lines) < 3:
            print(f"Error: Values are missing!")
            return None

        ypos = int(lines[0].strip())
        xpos = int(lines[1].strip())
        size = float(lines[2].strip())

        if ypos < 0 or ypos > 2:
            pass

        if xpos < 0 or xpos > 2:
            pass

        if size < 0 or size > 1:
            pass

        ypos = numpy.clip(ypos, 0, 2)
        xpos = numpy.clip(xpos, 0, 2)
        size = numpy.clip(size, 0, 1)

        return (xpos, ypos, size)

    except Exception as exception:
        print(f"ChatGPT error: {exception}")
        return None

#
# Add generated image placeholder in model
#
def model_image_gen(model, image_prompt):
    values = model_image_values_get(image_prompt)

    if not values:
        return None

    xpos, ypos, size = values

    height = len(model)
    width  = len(model[0])

    img_h = int(numpy.interp(size, [0, 1], [2, height - 3]))
    img_w = int(numpy.interp(size, [0, 1], [2, width  - 3]))

    # Space before image
    y_shift = ypos * (height - img_h) // 2
    x_shift = xpos * (width  - img_w) // 2

    # Space after image
    y_after = height - (y_shift + img_h)
    x_after = width  - (x_shift + img_w)

    if y_shift < 3 or y_after < 3:
        if y_shift < y_after:
            y_shift = 0

        else:
            y_shift += y_after

    if x_shift < 3 or x_after < 3:
        if x_shift < x_after:
            x_shift = 0

        else:
            x_shift += x_after

    for y in range(len(model)):
        for x in range(len(model[y])):
            if y >= y_shift and y <= (y_shift + img_h) and \
               x >= x_shift and x <= (x_shift + img_w):
                model[y][x] = 'X'

    return model

#
# Create empty model
#
def model_create(width, height):
    model = []

    for y in range(height):
        model.append([])

        for x in range(width):
            model[y].append(".")

    return model

#
# Save model to file
#
def model_save(model, model_file):
    try:
        with open(model_file, "w") as file:
            for row in model:
                line = ' '.join(row)

                file.write(line + '\n')

    except Exception as exception:
        print(f"korsord: Failed to write model file")

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
        help="Name of model"
    )

    parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing model"
    )

    args = parser.parse_args()


    # Disregard too small size
    if args.width < 3 or args.height < 3:
        print(f"korsord: {args.width}x{args.height}: Size too small")
        sys.exit(0)


    model_file = model_file_get(args.name)

    if os.path.exists(model_file) and not args.force:
        print(f"korsord: {args.name}: Model already exists")
        sys.exit(0)


    print(f"Generating model...")

    model = model_create(args.width, args.height)

    if args.image:
        model = model_image_gen(model, args.image)

    model_save(model, model_file)

    print(f"Generated model '{args.name}'")

    print(f"Done")
