# Repository Guidelines

## Project Structure & Module Organization

This repository contains C++ data structure lessons organized by `ChapterN/`. Keep each chapter's implementation, examples, tests, and teaching notes together. Template implementations generally live in `.hpp` files; standalone programs use `.cpp` files. For example, `Chapter14/binarySearchTree.hpp` is exercised by `Chapter14/testChapter14.cpp` and demonstrated by `Chapter14/chapter14Demo.cpp`. Each chapter's `README.md` explains its algorithms and local commands. There is no shared asset directory or repository-wide build system.

## Build, Test, and Development Commands

Build a chapter program from the repository root with a C++11 compiler:

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror Chapter14/testChapter14.cpp -o Chapter14/testChapter14.exe
.\Chapter14\testChapter14.exe
```

Replace the source and output paths to run another `test*.cpp` or `*Demo.cpp`. Consult that chapter's README for its complete command list. Generated `.exe` files are ignored by Git.

## Coding Style & Naming Conventions

Use four spaces for indentation and keep new code compatible with C++11. Match nearby names: classes and functions use mixed or lower camel case (for example, `binarySearchTree` and `bestFitBinPacking`); test files start with `test`. Put template definitions in headers and add include guards. Include required standard headers explicitly. Explain structural invariants, ownership, exceptional cases, and complexity in comments or the chapter README. Preserve established abstract data type interfaces when extending a chapter.

## Testing Guidelines

Tests are standalone executables using `assert`, without a separate test framework. Compile without `-DNDEBUG`, which disables those checks. Add a focused test for each new operation and meaningful edge cases such as empty structures, duplicate keys, deletion shapes, and invalid input. Run the changed chapter's existing tests and demo before submitting; successful tests usually print nothing.

## Commit & Pull Request Guidelines

Recent commits use descriptive chapter-focused messages, but no strict prefix format is enforced. Use a concise subject such as `Chapter14: add indexed BST applications`. Keep commits scoped to the relevant chapter. In a pull request, summarize the algorithm and interface changes, list the exact build and test commands run, link any related issue, and mention important complexity or behavior limits. Include screenshots only when a change has visual output.
