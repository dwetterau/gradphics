
uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D decal;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 objectToWorld;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 halfAngle;
varying vec3 c0, c1, c2;

void main()
{
  vec3 N = vec3(0.0, 0.0, 1.0);
  vec3 e = normalize(eyeDirection);
  vec3 reflectDir = reflect(-e, N);
  mat3 toObject = mat3(c0, c1, c2);
  vec3 worldDir = objectToWorld * toObject * reflectDir;
  gl_FragColor = textureCube(envmap, worldDir);
}
