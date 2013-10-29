
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
  // bumpy-diffuse
  vec3 ld = normalize(lightDirection);
  vec3 N = texture2D(normalMap, normalMapTexCoord).rgb;
  N = (N - vec3(0.5, 0.5, 0.5)) * 2.0;
  float diff_contribution = max(0.0, dot(ld, N));
  vec4 diffuseContrib = diff_contribution * LMd;

  // decal
  vec4 decalContrib = diff_contribution * texture2D(decal, vec2(-normalMapTexCoord.x, normalMapTexCoord.y)).rgba;
  decalContrib = vec4(decalContrib.r*LMd.r,
                      decalContrib.g*LMd.g,
                      decalContrib.b*LMd.b,
                      decalContrib.a*LMd.a);

  // specular
  float spec_contribution = max(0.0, pow(dot(halfAngle, N), shininess));
  vec4 specularContrib = LMs * spec_contribution;

  gl_FragColor = decalContrib + specularContrib + LMa;
}
