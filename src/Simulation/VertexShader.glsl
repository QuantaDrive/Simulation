#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 PosModelSpace;
layout (location = 1) in vec3 NormalModelSpace;
layout (location = 2) in vec3 Color;

out vec3 PosWorldSpace;
out vec3 NormalCameraSpace;
out vec3 EyeDirCameraSpace;
out vec3 LightDirCameraSpace;
out vec3 DiffuseColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 LightPosWorldSpace;

void main() {
    gl_Position =  MVP * vec4(PosModelSpace, 1);

    // Position of the vertex, in worldspace : M * position
    PosWorldSpace = (M * vec4(PosModelSpace, 1)).xyz;

    // Normal of the the vertex, in camera space
    NormalCameraSpace = ( V * M * vec4(NormalModelSpace, 0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 PosCameraSpace = ( V * M * vec4(PosModelSpace, 1)).xyz;
    EyeDirCameraSpace = normalize( - PosCameraSpace );

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosCameraSpace = ( V * vec4(LightPosWorldSpace, 1)).xyz;
    LightDirCameraSpace = LightPosCameraSpace + EyeDirCameraSpace;

    DiffuseColor = Color;
}

