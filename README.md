# `mfinit`

A modified version of [Rich Felker's minimal `init`](https://ewontfix.com/14/).

## Suitability

Not suitable for use in anything at all, unless you judge for yourself that it is. I do, however, make the claim that such judgement should be relatively straightforward.

## Portability

Should be portable to just about anything even remotely modern. Your compiler needs to support C89 or later.

## Dependencies

A `libc` for `sigfillset(3)` and various syscall wrappers.

## Errors

### Of the parent process

- When not launched as PID 1, returns 1.
- On failure of signal manipulations, returns 2 or 3.
- On failure to `fork(2)`, returns 4.
- Further failures are ignored.

### Of the child process

- On failures of signal manipulations or setting IDs, returns 5, 6, or 7.
- On failure to `exec`, returns -1.

## Changes

### Robust error checking

It now checks for error conditions robustly. It can't really handle them, other than exiting with an error code.

### Minor style changes

A few minor style changes, relating to constants and return values.
