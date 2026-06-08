# Contributing to corex

Thanks for your interest in improving corex! This is a small, focused library,
so contributions should respect its scope and style.

## Scope

corex is a **foundation kernel**: math, geometry, collision, transforms, and
cameras. It is intentionally *not* a renderer, physics engine, or game engine.
Please keep additions within the foundation layer. Higher-level features belong
in projects that depend on corex, not in corex itself.

## Ground rules

- **Single header.** All code lives in `corex.h`. Declarations go in the include
  guard; implementations go under `#ifdef COREX_IMPLEMENTATION`.
- **C99, no dependencies** beyond the C standard library. Must also compile as
  C++ (everything stays inside the `extern "C"` block).
- **No macros except configuration.** Prefer real functions over function-like
  macros.
- **Naming is strict:** types like `vec2`, `mat3`, `transform2d`; functions like
  `vec2_add`, `mat3_mul`, `transform2d_matrix`. No prefixes or namespaces beyond
  the type name.
- **Determinism and correctness first**, micro-optimizations later (and only with
  measurements).
- Document every new public function with a brief comment, and add it to
  `docs/api_reference.md`.

## Building and testing

```bash
make run        # build and run all examples
./build/collision_demo   # acts as the test suite (non-zero exit on failure)
```

CI builds on Linux/macOS (gcc + clang) and Windows (MSVC) with warnings treated
as errors, and also compiles a `COREX_USE_DOUBLE=1` configuration. Please make
sure:

- `make CC=gcc CFLAGS="-std=c99 -Wall -Wextra -Werror -pedantic -I."` is clean.
- `collision_demo` reports `0 failed`.
- New behavior is covered by a check in `examples/collision_demo.c` (or a new
  example) where practical.

## Pull requests

1. Keep PRs small and focused on one change.
2. Describe *why* the change is needed, not just *what* it does.
3. Update docs and the `CHANGELOG.md` "Unreleased" section.

By contributing, you agree that your contributions are licensed under the
[MIT License](LICENSE).
