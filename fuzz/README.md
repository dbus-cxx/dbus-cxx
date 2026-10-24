# Fuzz Testing

This directory contains [libFuzzer](https://llvm.org/docs/LibFuzzer.html)
based fuzz tests for dbus-cxx.  They are not built by default and are not part
of the unit tests; they are meant to be run manually.

| Fuzzer | What it tests |
|--------|---------------|
| `fuzz-message-create_from_data` | `DBus::Message::create_from_data`, i.e. parsing a raw message received from the bus |
| `fuzz-message-body` | Extracting every argument from the body of a parsed message with `DBus::MessageIterator`, including variants and nested containers |
| `fuzz-signature` | Parsing signature strings with `DBus::Signature` and `DBus::SignatureIterator`.  For signatures that `DBus::Validator::validate_signature` accepts, it also checks that the parsed tree has the structure defined by the DBus specification |

## Building

Fuzzing requires clang, since libFuzzer is part of LLVM.  Enable it with the
`ENABLE_FUZZING` CMake option.  It is best to use a separate build directory,
since the whole library gets built with fuzzing instrumentation and the
address and undefined behavior sanitizers:

```
mkdir build-fuzz
cd build-fuzz
CC=clang CXX=clang++ cmake -DENABLE_FUZZING=ON -DCMAKE_BUILD_TYPE=Debug ..
make fuzz-message-create_from_data
```

If clang is not on your `PATH`, give the full path to it instead (e.g.
`CXX=/usr/lib/llvm-14/bin/clang++`).

## Running

Each fuzzer has a set of seed inputs in `corpus/<fuzzer-name>/`.  These are
valid messages that give the fuzzer a starting point to mutate from.

libFuzzer takes a list of corpus directories.  New interesting inputs are
written to the first directory, so give it a scratch directory first and the
seed directory second so that the seeds in the source tree are not modified:

```
mkdir -p corpus crashes
./fuzz/fuzz-message-create_from_data -artifact_prefix=crashes/ \
    corpus ../fuzz/corpus/fuzz-message-create_from_data
```

The fuzzer runs until it finds a crash or you stop it with Ctrl-C.  Some
useful options:

* `-max_total_time=<seconds>` - stop after the given amount of time
* `-jobs=<N> -workers=<N>` - run N fuzzing processes in parallel
* `-fork=<N> -ignore_crashes=1` - keep going after a crash, saving each one
* `-max_len=<bytes>` - maximum size of a generated input (default 4096)

Run with `-help=1` to see all of the options.

## Crashes

When a crash is found, the input that caused it is saved as
`crashes/crash-<sha1>` and a stack trace is printed.  To reproduce it, pass the
file to the fuzzer:

```
./fuzz/fuzz-message-create_from_data crashes/crash-<sha1>
```

Since the fuzzer is a normal executable, it can also be run under gdb with the
crash file as its argument to debug the problem.

Once a crash is fixed, consider adding the input to the seed corpus in this
directory so that it is checked the next time the fuzzer is run.

## Adding seeds

Seeds are just binary files containing a single input.  To add one, copy it
into `corpus/<fuzzer-name>/`.  To shrink a large corpus that has been
generated down to a minimal set of files that has the same coverage, use
`-merge=1`:

```
mkdir minimized
./fuzz/fuzz-message-create_from_data -merge=1 minimized corpus
```

## Adding a new fuzzer

1. Create `<fuzzer-name>.cpp` in this directory that defines
   `extern "C" int LLVMFuzzerTestOneInput( const uint8_t* data, size_t size )`
2. Add `dbus_cxx_add_fuzzer( <fuzzer-name> )` to `CMakeLists.txt`
3. Optionally, add seed inputs to `corpus/<fuzzer-name>/`
