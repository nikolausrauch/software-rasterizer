#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

/* vertex data -> input to draw call (via Buffer) */
struct Vertex
{
    Vec3 pos;
    Vec3 color;
};

/*
 * Output of vertex stage, Input to fragment stage
 * -> position is mandatory
 * -> members to interpolate are declared by VARYING and VAL Macros (member need scalar multiplication, and addition)
*/
struct Varying
{
    Vec4 position;
    Vec3 color;

    VARYING( position, color );
};

/* uniform struct accessable from both "shaders" */
struct Uniforms
{
    Mat4 view;
    Mat4 proj;
};

int main(int argc, char** argv)
{
    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        out.position = uniform.proj * uniform.view * Vec4(in.pos, 1.0f);
        out.color = in.color;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        out = Vec4(in.color, 1.0);
    });

    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0f, 7.0f);
    uniforms.view = Mat4::translation(-Vec3{0.0f, 0.25f, 3.5f}) * Mat4::rotationX(radians(10.0f));

    /*========== Setup Buffer Data ========*/
    BufferIndexed<Vertex, unsigned int> buffer;
    buffer.primitive = ePrimitive::TRIANGLES;
    buffer.vertices = { {{-1.0,   0.0, -1.0},   {1.0, 0.0, 0.0} },
                        {{-1.0,   0.0,  1.0},   {1.0, 1.0, 1.0} },
                        {{ 1.0,   0.0,  1.0},   {0.0, 0.0, 1.0} },
                        {{ 1.0,   0.0, -1.0},   {0.0, 1.0, 0.0} } };
    buffer.indices = { 0, 1, 2,     2, 3, 0 };


    /* rasterizer with framebuffer size */
    Renderer rasterizer(1280, 720);

    /* clear framebuffer */
    rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

    /* submit draw call */
    rasterizer.draw(program, buffer);

    /* save framebuffer as .png */
    rasterizer.framebuffer().color().save("01_indexed_plane.png");

    return EXIT_SUCCESS;
}
