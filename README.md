# Korsord

Swedish crossword generator

## Memory

Changing from reallocating 1 more element to doubling capacity:

2,234,105 allocs, 14,411,689,117 bytes allocated
1,497,152 allocs, 213,428,461    bytes allocated

## Valgrind

```bash
valgrind --leak-check=no --track-origins=no --leak-resolution=med ./korsord
```

## Gprof

```bash
gprof ./korsord gmon.out > profile.txt
```

## AddressSanitizer

I have not been successful yet with getting address sanitizer to work

Enable address sanitizer

```bash
export ASAN_OPTIONS=verbosity=2
```

Disable address sanitizer

```bash
unset ASAN_OPTIONS
```

Allows core dumps

```bash
ulimit -c unlimited
```

## Debug

See the debug messages of program

```bash
tail -f output.txt
```

Add this line of code to exit program

```bash
kill(0, SIGINT);
```
