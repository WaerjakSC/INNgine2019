#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

uniform mat4 vMatrix;
uniform mat4 pMatrix;
// Values that stay constant for the whole mesh.
uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main()
{
        float particleSize = xyzs.w; // because we encoded it this way.
        vec3 particleCenter_worldspace = xyzs.xyz;

        vec3 vertexPosition_worldspace =
                particleCenter_worldspace
                + cameraRight * squareVertices.x * particleSize
                + cameraUp * squareVertices.y * particleSize;

        // Output position of the vertex
        gl_Position = pMatrix * vMatrix * vec4(vertexPosition_worldspace, 1.0f);

        // UV of the vertex. No special space for this one.
        UV = squareVertices.xy + vec2(0.5, 0.5);
        particlecolor = color;
}
