# Repository Guidelines

## Project Structure & Module Organization

This repository contains C++ data structure lessons organized by `ChapterN/`. Keep each chapter's implementation, examples, tests, and teaching notes together. Template implementations generally live in `.hpp` files; standalone programs use `.cpp` files. For example, `Chapter14/binarySearchTree.hpp` is exercised by `Chapter14/testChapter14.cpp` and demonstrated by `Chapter14/chapter14Demo.cpp`. Each chapter's `README.md` is the authoritative teaching handout. `tools/verify-all.ps1` is a lightweight repository-wide compile-and-run check; chapters remain independently buildable and there is no generated build tree in the repository.

## Build, Test, and Development Commands

Build a chapter program from the repository root with a C++11 compiler:

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror Chapter14/testChapter14.cpp -o Chapter14/testChapter14.exe
.\Chapter14\testChapter14.exe
```

Replace the source and output paths to run another `test*.cpp` or `*Demo.cpp`. Consult that chapter's README for its complete command list. Generated `.exe` files are ignored by Git.

Run the full executable teaching corpus and standalone-header checks without writing binaries into the repository:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/verify-all.ps1
```

Keep the script's explicit multi-source mappings current when a program gains a non-header implementation file.

## Coding Style & Naming Conventions

Use four spaces for indentation and keep new code compatible with C++11. Match nearby names: classes and functions use mixed or lower camel case (for example, `binarySearchTree` and `bestFitBinPacking`); test files start with `test`. Put template definitions in headers and add include guards. Include required standard headers explicitly. Explain structural invariants, ownership, exceptional cases, and complexity in comments or the chapter README. Preserve established abstract data type interfaces when extending a chapter.

## Testing Guidelines

Tests are standalone executables using `assert`, without a separate test framework. Compile without `-DNDEBUG`, which disables those checks. Add a focused test for each new operation and meaningful edge cases such as empty structures, duplicate keys, deletion shapes, and invalid input. Run the changed chapter's existing tests and demo before submitting; successful tests usually print nothing.

## Commit & Pull Request Guidelines

Recent commits use descriptive chapter-focused messages, but no strict prefix format is enforced. Use a concise subject such as `Chapter14: add indexed BST applications`. Keep commits scoped to the relevant chapter. In a pull request, summarize the algorithm and interface changes, list the exact build and test commands run, link any related issue, and mention important complexity or behavior limits. Include screenshots only when a change has visual output.

## Teaching Handout Contract

Treat code and prose as one deliverable. A chapter README should let a student with the listed prerequisites understand and verify the chapter without undocumented lecture commentary. Preserve this structure when adding or substantially changing a topic:

1. State learning objectives, prerequisites, file-reading order, and the exact problem specification.
2. Define the ADT and representation invariants before implementation details. Explain ownership, pointer/reference validity, comparator requirements, and invalid-input behavior where relevant.
3. Give at least one concrete, step-by-step trace using actual values. For multi-algorithm chapters, trace each central algorithm rather than substituting one generic template.
4. Explain why the algorithm is correct using the appropriate device: loop invariant, structural induction, exchange argument, cut property, state recurrence, or safe upper/lower bound.
5. Report time and extra-space costs for the implementation in the repository. Name all size parameters and assumptions. Include preprocessing, result sorting, copied adjacency lists, state-vector copies, recursion depth, and output size when they affect the bound.
6. Cover meaningful boundaries such as empty input, one element, duplicates, invalid indices, arithmetic overflow, disconnected graphs, missing paths, negative edges, negative cycles, and no-solution cases.
7. List exact PowerShell commands from the repository root. Include every required `.cpp` source and keep `assert` enabled.
8. End with tiered exercises and concise answer hints, plus a connection to adjacent chapters.

Do not preserve obsolete bug-history prose in student-facing handouts. Describe the current invariant and behavior. Git history remains the source for historical changes.

## Truthfulness and Coverage

Distinguish these states explicitly:

- **Implemented and tested:** the interface exists and a relevant test or executable exercises it.
- **Demonstrated:** an example exists, but it is not a reusable general implementation.
- **Concept only:** useful extension material that has no implementation in this repository.
- **Not covered:** outside the current course scope.

Do not call an expected or average bound a worst-case guarantee. Do not call a heuristic optimal. Do not describe an ADT target cost as the cost of a concrete implementation without checking the code. If a result depends on representation, comparator, hash distribution, numeric range, graph connectivity, nonnegative weights, or randomization, state that condition next to the claim.

When reviewing a README, reread the current header and tests before editing. Verify symbol names, index conventions, tie rules, exception behavior, output ordering, and commands. Prefer links relative to the README inside repository Markdown.

## Algorithm and Data Structure Review Checklist

Before considering a chapter change complete, check:

- The representation invariant is preserved after every public mutator.
- Empty, singleton, duplicate, boundary-index, failure, and no-solution behavior are explicit.
- Recursive algorithms have a terminating base case and include call-stack space.
- Pointer-owning classes follow correct copy/move/destruction semantics or deliberately disable copying.
- Returned pointers, references, and iterators have documented invalidation rules.
- Arithmetic that accumulates weights, costs, dimensions, ranks, or capacities has an appropriate type and overflow policy.
- Randomized, amortized, expected, pseudopolynomial, output-sensitive, and worst-case bounds are labeled accurately.
- Tests assert observable behavior and invariants, not only demonstration output.
- The changed chapter's tests and demos pass with strict warnings; then run `tools/verify-all.ps1` for repository-wide changes.

## Documentation and Code Synchronization

Every interface or behavior change must update the chapter README in the same change. Every complexity correction should identify the implementation detail that changes the bound. Every new major algorithm needs a focused test, a readable demo when output aids learning, and an entry in the root course map if it changes chapter scope.

Keep repository-wide plans under `docs/`; keep temporary logs, binaries, compiler output, and generated timing data outside the repository. Do not commit `.exe` files.
