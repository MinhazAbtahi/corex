# Changelog

All notable changes to this project are documented here. The format is based on
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.0.0] - 2026-06-08

### Added

- Initial release of the single-header `corex.h` foundation kernel.
- **Types:** `vec2`, `vec3`, `vec4`, `mat3`, `mat4`, `rect`, `circle`,
  `transform2d`, `camera2d`, `color`.
- **Vector math (vec2):** construction, arithmetic, dot/cross, length,
  normalize, lerp, distance, perpendicular, rotate.
- **Matrix system (mat3):** identity, translation, rotation, scale, multiply,
  point transform; plus `mat4_identity` for future 3D use.
- **Geometry:** `rect_new`, `circle_new`, `rect_center`, `color_new`.
- **Collision:** point↔rect, point↔circle, rect↔rect, circle↔circle,
  rect↔circle.
- **Transform:** `transform2d_matrix` (`T * R * S`).
- **Camera:** `camera2d_view`, `camera2d_world_to_screen`,
  `camera2d_screen_to_world` (exact analytic inverse).
- **Utility math:** `clamp`, `lerp`, `inverse_lerp`, `smoothstep`,
  `deg_to_rad`, `rad_to_deg`.
- Configuration hooks: `COREX_USE_DOUBLE`, `COREX_STATIC`, `COREX_ASSERT`,
  `COREX_MALLOC`, `COREX_FREE`.
- Examples: `pong`, `camera_demo`, `collision_demo`.
- Documentation: `docs/design.md`, `docs/api_reference.md`.

[Unreleased]: https://github.com/MinhazAbtahi/corex/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/MinhazAbtahi/corex/releases/tag/v1.0.0
