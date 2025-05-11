# Notes

## Quick Fixes
- change all args.REMAINDER to string.split(' ')
- add 'words' to render-gen.py
- add 'words' arg to gen.py, which consists of pre generated words (in priority order)
- add 'model', 'grid', 'clues', 'image' which gets overwritten at each step
- change 'theme' to 'prompt' in gen.py. Theme, image_pos and image_prompt is then extracted from prompt
- don't print "Använd inte följande ord:" om inga ord finns

## korsord.c
- return proper exit code on fail in korsord.c
- README.md in source/ explain the algorithm and refrence function names
- Delete debug.sh and valgrind.sh

## Words
- only read first word of each line in trie_load in korsord.c
- make it possible to read word files with existing clues (word : clue)
