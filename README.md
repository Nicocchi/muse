
# μ's Editor

A small C terminal editor inspired by [Bisqwit's](https://www.youtube.com/user/Bisqwit) [*That Editor*](https://github.com/bisqwit/that_editor)

### Installation

Linux, Mac OS X, Windows
* Clone the repository
* Move into the directory
* Run the `make` command
* Run the application with `./muse`

```

$ git clone https://github.com/Nicocchi/muse.git
$ cd /muse
$ make
$ ./muse

```

### Usage

```
$ muse <filename>
```

### Keys

```
CTRL-S: Save
CTRL-Q: Quit
CTRL-F: Find string in file (ESC to exit search, arrows to navigate)
```

### Syntax Highlighting

Languages currently supported for syntax highlighting

```
C and CPP
Python
```

### Requirements

C Compiler: [GCC](https://gcc.gnu.org) (Recommended) or [MingGW](http://www.mingw.org) for Windows

# License

The base code for this project was [originally written](https://github.com/antirez/kilo) by Salvatore Sanfilippo (aka antirez) and released under the BSD 2-clause license (see LICENSE.original). I've made changes to the original.

  

All other files in this repository are licensed under [MIT](https://opensource.org/licenses/MIT)