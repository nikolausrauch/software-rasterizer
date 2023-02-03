# CPU Software Rasterizer

A simple software-rasterizer in C++, that I wrote to get a basic understanding of the OpenGL graphics pipeline.

## Features
- C++ implementation
  - generic vertex and fragment attributes
  - programmable vertex and fragment shader (functors)
  - small math library for handling 2D, 3D, 4D vectors and 2x2, 3x3, 4x4 matrices
  - .obj and .mat loading
  - GLFW/OpenGL viewer (uploads framebuffer each frame)
- perspective-correct attribute interpolation
- z-buffering
- texture mapping with filter (nearest, linear)

## Examples

### Minimal setup (colored triangle)

Similar to traditional Graphic APIs we define the input and output of the vertex shader stage; the data passing through the rasterization pipeline.

All data members from `Varying` contained in the `VARYING(...)` macro are automatically interpolated per fragment by the rasterizer. 
```cpp
/* vertex data -> input to draw call (via Buffer) */
struct Vertex
{
    Vec3 pos;
    Vec3 color;
};

/*
 * Output of vertex stage, Input to fragment stage
 * -> position is mandatory
 * -> members to interpolate are declared by VARYING macro (member need scalar multiplication, and addition)
*/
struct Varying
{
    Vec4 position;
    Vec3 color;

    VARYING(position, color);
};

/* uniform struct accessable from both "shaders" */
struct Uniforms {};
```
Vertex and fragment shaders are written as function objects which need to be set accordingly with `onVertex` and `onFragment` (currently no default shader). 
```cpp
Program<Vertex, Varying, Uniforms> program;
program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
{
    out.position = Vec4(in.pos, 1.0f);
    out.color = in.color;
});

program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
{
    out = Vec4(in.color, 1.0f);
});
```

Mesh data is provided to the renderer with a `Buffer` object.

```cpp
Buffer<Vertex> buffer;
buffer.primitive = ePrimitive::TRIANGLES;
buffer.vertices = { { {-0.5, -0.5, 0.5}, {1.0, 0.0, 0.0} },
                    { { 0.5, -0.5, 0.5}, {0.0, 1.0, 0.0} },
                    { { 0.0,  0.5, 0.5}, {0.0, 0.0, 1.0} } };

```
An instance of `Renderer` contains a default framebuffer with a color and depth target.

After clearing the framebuffer, we can submit a draw call with the previously defined shader program and vertex buffer.

<img src="img/00_triangle.png" align="right" height=250px>

```cpp
/* rasterizer with framebuffer size */
Renderer rasterizer(480, 480);

/* clear framebuffer */
rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

/* submit draw call */
rasterizer.draw(program, buffer);

/* save framebuffer as .png */
rasterizer.framebuffer().color().save("00_triangle.png");

```
### Results

Model Loading and Texture Mapping ("Sad toaster" [Link](https://skfb.ly/on9Dn) by tasha.lime.)

![alt](img/example_model.jpg)

Blinn-Phong Illumination (Phong Shading)

![alt](img/example_blinn_phong.jpg)

## :books: Useful Resources
[Tiny Renderer](https://github.com/ssloy/tinyrenderer/wiki/Lesson-0:-getting-started)

[Scratchpixel Lesson](https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/overview-rasterization-algorithm.html)

[Rasterator](https://github.com/diharaw/Rasterator)

[SRPBR](https://github.com/niepp/srpbr)

[Stack Overflow Perspective Interpolation](https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation)
