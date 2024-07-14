# TerminaR
Terminal Renderer. STB-style single header library.

Before including the library, define the implementation: ```#define TERMINAR_IMPLEMENTATION```

Every cell in the terminal is equivalent to 2 pixels. This is because ANSI escape codes allow you to change the foreground and background of each cell.

Each function and macro is prefixed by ```trmR_```
