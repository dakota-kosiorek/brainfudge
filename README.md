# brainfudge
 A command line brainfuck (bf) interpreter written in C for Windows and Linux. There are many other great resources to learn about the languages (I like this one the most https://gist.github.com/roachhd/dce54bec8ba55fb17d3a), but it would be good to explain the basics of the language and the rules of this interpreter.

 There are a certain number of cells (by default 32768) that can hold a value between 0-255. At the beginning of the program the value of all these cells are set to 0. There is also a memory and instruction pointer. At the start of the program the memory pointer points to the first memory cell and the instruction pointer points to the first valid command in a bf script. There are 8 commands in bf:
 ```
 '>' Move the memory pointer to the right cell
 '<' Move the memory pointer to the left cell
 '+' Increment the cell value at the memory pointer
 '-' Decrement the cell value at the memory pointer
 '.' Output the character signified by the cell at the memory pointer
 ',' Input a character and store it in the cell at the memory pointer
 '[' Jump past the matching ] if the cell at the pointer is 0
 ']' Jump back to the matching [ if the cell at the pointer is nonzero
 ```
 Any other character besides these 8 are treated as comments by the interpreter. If the memory pointer tries to be moved to left when it currently points towards the 0th cell or the memory pointer tries to be moved to right when at the last cell, the memory pointer does not move. If the value of the cell is incremented at 255 or decremented at 0, the cell value overflows to 0 and 255 respectively. Loops can be nested as many times as the user wants, but the interpreter does not check if there is a corresponding closing bracket to the loop.

# Usage
Compile using the makefile and run:

Windows
```
brainfudge filename
```

Linux
```
./brainfudge filename
```

There are two additional parameters that can be added when running the interpreter `-c` and `-d`. `-c` should be followed with a interger (e.x. `-c 16`) and is used to specify the number of memory cells for the program. `-d` dumps the contents of each memory cell at the end of the programs execution.

# Examples
This script prints "Hello World!".
```
./braunfudge examples/hello.bf
```

This script prints "Hello World!", specifies that the program should have 8 memory cells, and then dumps the content of those cells at the end of program execution.
```
./braunfudge examples/hello.bf -c 8 -d
```

This script prints each character the user inputs directly after the user inputs them until the enter key is pressed, specifies that the program should have 8 memory cells, and then dumps the content of those cells at the end of program execution.
```
./braunfudge examples/cat.bf -c 8 -d
```

This script prints each character the user inputs directly after the user inputs them until the enter key is pressed, saves each character that is entered into a memory cell, specifies that the program should have 8 memory cells, and then dumps the content of those cells at the end of program execution.
```
./braunfudge examples/string.bf -c 32 -d
```

## License
[The MIT License (MIT)](http://opensource.org/licenses/mit-license.php)
