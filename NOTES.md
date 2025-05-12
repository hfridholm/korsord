# Notes
- remove unnecessary print statements
- add good print statements
- change all args.REMAINDER to string.split(' ')
- add 'words' to render-gen.py
- add 'image' to gen.py
- change 'theme' to 'prompt' in gen.py. Theme, image_pos and image_prompt is then extracted from prompt
- don't print "Använd inte följande ord:" om inga ord finns
- return proper exit code on fail in korsord.c
- Delete debug.sh and valgrind.sh

## Maybe
- only read first word of each line in trie_load in korsord.c
- make it possible to read word files with existing clues (word : clue)
