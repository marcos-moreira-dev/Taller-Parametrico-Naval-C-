# Professionalization Roadmap

## Current direction

The project already has a strong base:

- wxWidgets desktop UI
- split between presentation, application, domain, simulation, persistence, education
- 2D and 3D rendering modes
- editable scenarios and configurable vector fields
- educational theory content externalized in UTF-8 assets

The next step is not adding more features blindly, but stabilizing architecture and reducing technical debt.

## Equivalent roadmap in C++

### 1. Error model by layer

- Domain: validation errors and invariant failures
- Application: orchestration errors and user-facing translation
- Persistence: file format, path, parsing and serialization errors
- Presentation: convert exceptions into dialogs/toasts/status messages

Recommended approach:

- throw exceptions only in low-level/domain/persistence boundaries
- catch and translate at application/presentation boundaries
- define a small hierarchy of project exceptions

### 2. Structured logging

Recommended equivalent of slf4j/logback in C++:

- keep the current logger short-term
- medium term migrate to `spdlog`

Track at least:

- scenario preset selected
- field type/intensity/center
- simulation start/pause/reset
- numerical method and timestep
- parser failures for custom fields
- export and persistence operations

### 3. Formal configuration model

`ExperimentConfig` is already close to this, but should become the single typed source of truth.

Complete it with explicit UI-facing state for:

- field type
- field intensity
- field center
- render-layer visibility
- scenario metadata

### 4. Stronger tests

Missing high-value tests:

- preset loading consistency
- custom field parser behavior
- scenario resize/persistence roundtrip
- simulation end-to-end for Euler / Improved Euler / RK4
- theory asset loading and UTF-8 sanity

### 5. Smaller presentation contracts

Right now several panels still know too much.

Goal:

- `MainWindow` orchestrates
- `RightPanel` edits typed config
- `LeftPanel` selects scenario/layers
- `TheoryPanel` only renders theory content
- canvas classes only render state

### 6. Less inferred UI state

Never infer core semantics from labels or sampled field vectors if you can store them explicitly.

Already started with `fieldView`.

Keep pushing this idea.

### 7. Telemetry and performance

Measure and log:

- simulation step time
- 2D redraw time
- 3D redraw time
- field overlay draw cost
- memory growth of trajectory and graph buffers

### 8. Operational documentation

Still needed:

- release build guide
- GitHub first publish guide
- known limitations
- custom field syntax guide

### 9. Compatibility policy

Document explicitly:

- supported compiler/toolchain
- supported Windows version
- asset format compatibility
- save-file compatibility policy

### 10. CI / repository hygiene

Recommended first CI checks:

- configure with CMake
- build GUI
- run tests
- package artifacts

## Preset architecture target

Short term: native C++ presets are acceptable.

Medium term: move presets to data files.

Suggested structure:

- scenario geometry asset
- field expression asset
- metadata asset

Example:

```text
assets/presets/
  01_rio_uniforme/
    meta.json
    scenario.txt
    field.fx.txt
    field.fy.txt
```

Then the software can:

- parse the equations
- build the vector field
- draw arrows consistently in 2D and 3D
- keep presets data-driven and cleaner

## Immediate refactor priorities

1. Unify field overlay generation for 2D and 3D
2. Introduce scenario document/editor state
3. Strengthen custom expression parsing and validation
4. Add tests for presets and UTF-8 theory assets
5. Replace ad-hoc logging with structured logging
