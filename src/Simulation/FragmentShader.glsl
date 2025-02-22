#version 330 core

// Interpolated values from the vertex shaders
in vec3 PosWorldSpace;
in vec3 NormalCameraSpace;
in vec3 EyeDirCameraSpace;
in vec3 LightDirCameraSpace;
in vec3 DiffuseColor;

// Output data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform vec3 LightPosWorldSpace;

void main() {
    float LightPower = 1250.0f;

    // Material properties
    //vec3 DiffuseColor = vec3(1,0,0);
    vec3 AmbientColor = vec3(0.25,0.25,0.25) * DiffuseColor;
    vec3 SpecularColor = vec3(0.2,0.2,0.2);

    // Distance to the light
    float distance = length( LightPosWorldSpace - PosWorldSpace );

    // Normal of the computed fragment, in camera space
	vec3 n = normalize( NormalCameraSpace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirCameraSpace );
	// Cosine of the angle between the normal and the light direction,
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n, l ), 0, 1 );

    // Eye vector (towards the camera)
    vec3 E = normalize(EyeDirCameraSpace);
    // Direction in which the triangle reflects the light
    vec3 R = reflect(-l,n);
    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to 0
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    float cosAlpha = clamp( dot( E, R ), 0, 1 );

    color =
        // Ambient : simulates indirect lighting
        AmbientColor +
        // Diffuse : "color" of the object
        DiffuseColor * LightPower * cosTheta / (distance) +
        // Specular : reflective highlight, like a mirror
        SpecularColor * LightPower * pow(cosAlpha,5) / (distance);
}