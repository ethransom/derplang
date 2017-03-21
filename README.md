# Derplang

An absolutely pointless dynamic, interpreted, procedural language. It has a
garbage collector, though. I've been told that gives me hacker cred.

## Usage

```shell
$ make
$ ./main examples/hello_world.derp
```

## FAQ

#### Why do we need yet another language?

We don't.

#### What does Derplang bring to the table over Ruby, Python, or Javascript?

Absolutely nothing.

#### Does Derplang have objects?

No.

#### What can the standard library do?

`print` and `println`. That's it.

#### What the heck?

Yes.

#### What's planned for the language?

Thanks for asking! I'm just writing this because YOLO, so I'm probably going to
implement features that sound fun and won't be too much trouble. Here's what
I've thought about (in decreasing order of feasibility):

 * Classes and Objects
 * M:N threading. (Think Goroutines.)
 * Some sort of novel approach to... anything, really.
 * Super speed!
 * JIT compilation
 * Actual usefulness

#### Why is it in C?

With C you write code. With C++ you try and figure out how virtual methods work
or something.

#### Can I help?

Pull requests containing code are probably a no-no. This is mostly a personal
project that I'm writing for the fun of writing. However, I'd LOVE to hear
feedback on existing code. Tell me how my code sucks. Tell me how Derplang sucks.

## Conventions

 * Same-file helpers should always be marked as `static`.
 * Use the kernel style guide, except for:
   * Use typedefs for structs and friends.
   * Format curly braces around functions the same way you do control flow

