# Gory Emmental
Gory Emmental is an interpreter for the [Emmental](https://github.com/catseye/Emmental/) esoteric language, written in C++.

## Usage
### Interpreting a File
`GoryEmmental file` will interpret the file located at `file` as an Emmental program. Please note that the file will be interpreted in full, including tabs, spaces, and newline characters. If you don't want that, use the `-w` option (see more about options below).

### Using Interactive mode
`GoryEmmental -i` will launch the interpreter in *interactive mode*, where you can type Emmental programs and see their result in real-time. Interactive mode also has commands to help you, such as clearing the stack, resetting symbol definitions, checking current symbol definitions, and more.

## Runtime Options
These options can be combined with either the file interpretation or interactive mode. Additionally, they can be toggled in interactive mode with the `__toggle` command.

### `-i`, `--interactive`
This launches the Interpreter in Interactive Mode, as explained above.

### `-l`, `--lenient`
**Recommended in Interactive Mode**

Without this option, errors will instantly halt program execution and close the interpreter, even in Interactive Mode. With this option active, errors are "ignored", and the interpreter will *break standard Emmental behavior* to let the program keep running, such as returning **NUL** when an empty stack is popped.

### `-q`, `--quiet`
**Not recommended in Interactive Mode**

With this option active, the interpreter will print no warnings or errors, even in Interactive Mode: Only the program output will be printed.

### `-w`, `--nowhitespace`
Normally, the interpreter considers every single byte in the file as an Emmental symbol. With this option active, whitespace characters (space, tab, line feed, carriage return, etc) will be ignored when reading a file. This has no effect in Interactive Mode.

### `-o`, `--optimize`
**Recommended always**

This option allows the interpreter to *break the Emmental standard* to optimize the program, without altering its behavior. For example, if we have the definition `A ? B` and a program uses `;#65#67!` to create the mapping `C ? A`, the interpreter will instead create `C ? B`, as it is identical to the expected `C ? A ? B`, and saves one recursion level.

### `-c`, `--color`
Gory Emmental automatically uses [ANSI Color Codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors), on systems that support it, to colorize the interpreter output. This options allows you to invert the interpreter behaviour: Disable coloring on systems that support ANSI Color Codes, or force coloring on systems that don't.

### `-d`, `--debug`
**Not recommended for file interpretation**

With this option enabled, the Stack and the Queue will be outputted after each Symbol is interpreted.

## License
This project is under the MIT License. See the LICENSE file on the root directory for more info.
