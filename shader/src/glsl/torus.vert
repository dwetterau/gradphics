
attribute vec2 parametric;

uniform vec3 lightPosition;  // Object-space
uniform vec3 eyePosition;    // Object-space
uniform vec2 torusInfo;

varying vec2 normalMapTexCoord;

varying vec3 lightDirection;
varying vec3 halfAngle;
varying vec3 eyeDirection;
varying vec3 c0, c1, c2;

void main()
{
  
  float R = torusInfo.x;
  float r = torusInfo.y;
  float M_PI = 3.141592653589793;
  float theta = parametric.x * 2.0 * M_PI;
  float phi = parametric.y * 2.0 * M_PI;

  vec3 xyz = vec3((R + r*cos(phi))*cos(theta),
                  (R + r*cos(phi))*sin(theta),
                  r*sin(phi)
                  );
  gl_Position = gl_ModelViewProjectionMatrix * vec4(xyz, 1);  // XXX fix me

  normalMapTexCoord = vec2(6.0 * parametric.x, 2.0 * parametric.y);  // XXX fix me
  
  
  
  //eyeDirection = vec3(0);  // XXX fix me
  eyeDirection = normalize(eyePosition - xyz);

  //lightDirection = vec3(0);  // XXX fix me
  lightDirection = normalize(lightPosition - xyz);
 
  halfAngle = vec3(0);  // XXX fix me
  c0 = vec3(0);  // XXX fix me
  c1 = vec3(0);  // XXX fix me
  c2 = vec3(0);  // XXX fix me
}

