# Chip 8 Emulator

## Introduction
Recently I was curious about how emulators worked, and after reading up a bit, I discovered that implementing a chip 8 emulator was a good place to start. The project was completed in C++ 20, attempting to adhere to best practices of modern C++ as much as possible.

I used a couple of different sources to get by this project. The main guide that I followed was this [tutorial by Austin Morlan](https://austinmorlan.com/posts/chip8_emulator/). I also referred to [Tobias V. I. Langhoff's guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) at times for further clarifications.

While the guide I followed did contain the code implementations, I found myself implementing several portions pretty differently as the guide code was written in more C-like C++. The project also gave me a chance to explore make files and use external libraries within C++. I would greatly appreciate any feedback.

## Learning Outcomes
* CMake and writing makefiles,
* Using external libraries in C++,
* Exposure to graphics rendering and SDL,
* Polishing up on modern C++ best practices,
* Clearer understanding of bit-wise operations,
* Understanding how emulators work

## Usage
To run the program in your device, first ensure that you have a [C++ Compiler](https://code.visualstudio.com/docs/languages/cpp) (note: while this is a tutorial for vscode, installing the compiler for any other editor should follow the same process) and [CMake](https://cmake.org/download/) installed on your system.

Once you have cloned the repository, navigate to the root directory and run `cmake -B build`. This will generate the build files required for the project. Once that is done, navigate to the `build` directory and run `make` to compile. Once compiled, running `./chip8_emulator <Scale> <Delay> <ROM>` will run the emulator.
> **_NOTE:_**  if you are on windows, you may need to copy the `.dll` files for SDL into the same directory as your executeble (inside `build/`).

Please ensure that the file you use is a `.ch8` file. You can find an archive of files to test [here](https://github.com/dmatlack/chip8/tree/master/roms/games). Two `.ch8` files are already provided in the root directory.