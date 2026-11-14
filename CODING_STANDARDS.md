# Coding Standards — SDL3 Proto

This document defines the coding conventions used in the `Core/` directory and the project as a whole. These standards are derived from the existing codebase and the SDL3 project's style.

---

## 1. General

- **Language:** C++17 (or later, as required by SDL3)
- **File encoding:** UTF-8, no BOM
- **Line endings:** LF (`\n`) on all platforms
- **Indentation:** 4 spaces per indent level; never use tabs
- **Column limit:** No hard limit (0 in `.clang-format`)
- **Final newline:** Every source file must end with a single newline
- **Trailing whitespace:** Must not be present on any line

---

## 2. Naming Conventions

| Element | Style | Example |
|---|---|---|
| Classes / Structs | `PascalCase` | `SDLRenderer`, `ResourceManager` |
| Functions / Methods | `camelCase` (member), `snake_case` (free) | `getRenderer()`, `loadTexture()` |
| Member variables | `mCamelCase` prefix | `mWindow`, `mRenderer` |
| Constants / `static constexpr` | `ALL_CAPS_WITH_UNDERSCORES` | `WINDOW_WIDTH`, `WHITE_COLOR` |
| Namespaces | `PascalCase` or short `snake_case` | `Utils`, `SDL` |
| Typedefs / Aliases | `PascalCase` | `TexturePtr` |
| Enum values | `ALL_CAPS` | `SDL_WINDOW_RESIZABLE` |

---

## 3. Header Files (`.hpp` / `.h`)

- Use traditional `#ifndef __HEADER__HPP__` include guards.
- Include system headers first, then project headers, separated by a blank line.
- Do not include unnecessary headers; prefer forward declarations where possible.
- Keep includes minimal and specific.

**Example structure:**
```cpp
#ifndef __SDL_TEXTURE_2D__HPP__
#define __SDL_TEXTURE_2D__HPP__

#include <string>
#include <memory>

#include "SDLCore/SDLTexture2D.hpp"
```

#endif
---

## 4. Source Files (`.cpp`)

- Include the corresponding header first, then other project headers, then system headers.
- Use the `#include "Header.hpp"` form for project headers (with quotes, not angle brackets).

**Example structure:**
```cpp
#include "SDLCore/SDLTexture2D.hpp"
#include <SDL3/SDL_render.h>
```

---

## 5. Formatting (clang-format)

This project uses clang-format. The configuration is based on the SDL3 `.clang-format` file with the following key settings:

- **Indent width:** 4 spaces
- **Continuation indent width:** 4 spaces
- **Brace style:** Custom — opening brace on a new line for functions, classes, enums, structs, and namespaces; same line for control statements.
- **Pointer alignment:** Left (`int* ptr`)
- **Space after C-style cast:** No
- **Space in empty parentheses/braces:** No
- **Space around pointer qualifiers:** Default
- **Include blocks:** Preserve (do not reorder)

---

## 6. Class and Member Conventions

- Classes with a public-facing API (behavior) keep data members `private` and expose getter/setter methods per member (`getCenter()`/`setCenter(...)`).
- Public methods (including accessors) first, then `private:` data members.
- Data-only structs (config/constants, e.g. `SpriteConfig`, `KeyConfig`, `VehicleControl::Constants`) may keep public members — they carry no behavior.
- Use `override` on virtual overrides where applicable.
- Constructor initializer lists use the `: member(expr)` syntax on the same line as the constructor declaration.
- Destructors should be `virtual` in base classes.
- Use `const` on member functions that do not mutate state.
- Use `&` for references (not pointers) where null is not a valid value.

**Example:**
```cpp
class SDLRenderer
{
public:
    static SDLRenderer& getRenderer();

    void init(SDLWindow& window);
    void setClearColor(const SDL_Color& color);
    void clear();

    SDL_Renderer* getRendererContext() { return mRenderer; }

private:
    SDL_Renderer* mRenderer;

    SDLRenderer();
};
```

---

## 7. Namespaces

- Use namespaces to group related functionality.
- The `Utils` namespace is used for free utility functions (e.g., math helpers, file listing).
- Avoid `using namespace` in header files; it is acceptable in `.cpp` files for brevity if the namespace is small and unambiguous.

---

## 8. Comments

- Use `//` for single-line comments and `/* */` for multi-line comments.
- Use `// TODO:` for known issues or future work, and `// FIXME:` for known bugs.
- Doxygen-style `///` or `/** */` comments are used for public API documentation.

**Example:**
```cpp
// TODO: flush instead of draw, how do we handle framebuffers?
void SDLRenderer::draw()
{
    SDL_RenderPresent(mRenderer);
}
```

---

## 9. Resource Management and Smart Pointers

- Prefer `std::shared_ptr` for shared ownership (e.g., textures in the resource manager).
- Use `std::make_shared` for creating shared pointers.
- Raw pointers are acceptable for non-owning references (e.g., `SDL_Renderer*`).

---

## 10. Build System

- The project uses CMake (minimum version 3.16).
- Source files are listed explicitly in `CMakeLists.txt`.
- Include directories are set with `target_include_directories`.
- Compile commands are exported via `CMAKE_EXPORT_COMPILE_COMMANDS ON`.

---

## 12. What to Avoid

- Do not use C-style casts; use C++ casts (`static_cast`, `reinterpret_cast`, etc.).
- Do not use `using namespace std;` in header files.
- Do not use magic numbers; define named constants (`static constexpr`) instead.
- Do not leave commented-out code in committed files (use version control history instead).

---


*These standards are derived from the existing codebase in `Core/` and the SDL3 project conventions. When in doubt, match the surrounding code style.*
