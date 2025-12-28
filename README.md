***Project Objective***
As a Game Technician/Technical Artist candidate, my goal was to bridge to gap between 3D assets & web-based environment.
I implemented a modular loader system that minimizes boilerplate code while maintaining high visual fidelity & performance.

***Technical Features***
## 1.Physically Based Rendering(PBR) Lite
The renderer implements a PBR-inspired shading model to ensure visual consistency under various lighting conditions:
**Multi-Texture Support:** Samples BaseColor, Normal, Roughness, and Metallic maps.
**Gamma Correction:** Implemented 1/2.2 power-law encoding for linear space rendering.
**Dynamic Lighting:** Supports three distinct light types: Directional, Point, and Spot lights within a single shader.

## 2.Advanced Surface Detail & Debugging
**Normal Mapping:** Uses TBN(Tangent, Bitangent, Normal) matrices to transform tangent-space normals into world-space.
**Vertex Displacement & visualizer:** Includes a custom shader pair
(NormalVertexShader.hlsl, NormalPixelShader.hlsl) to visualize surface normals as lines for debugging geometry and tangent space.

## 3.Automated Asset Pipeline(Assimp integration)
**Format Versatility:** Utilizes the Assimp library to load complex 3D formats such as FBX.
**Smart Texture Mapping:** Automatically searches and binds textures based on material naming conventions.
(eg., detecting "BaseColor" and finding corresponding "ORM" or " Normal" maps).
**Geometry Generation:** includes a built-in GeometryGenerator for procedural primitives like boxes, spheres, and cylinders for engine-side testing.

***Tech Stack***
**Language:** C++17
**Graphics API:** Direct3D 11(HLSL SM 5.0)
**Math Library:** DirectXMath / SimpleMath
**External Libraries:** Assimp: For 3D model Parasing.
- ImGui: For real-time scene & material control.
- stb_image: For efficient texture loading.

***Technical Highlights: TBN Transformation***
One of the core challenges addressed was the correct implementation of normal mapping.
The BasicVertexShader passes world-space tangent and bitangent vectors to the pixel shader, where they are used to construct the TBN matrix.

$$TBN = \begin{bmatrix} T_x & B_x & N_x \\ T_y & B_y & N_y \\ T_z & B_z & N_z \end{bmatrix}$$

This allows the sampled normal from the texture to be correctly oriented in world space,
significantly enhancing visual fidelity without increasing vertex counts.
