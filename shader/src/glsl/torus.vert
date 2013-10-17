
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
  normalMapTexCoord = vec2(0);  // XXX fix me
  float R = torusInfo.x;
  float r = torusInfo.y;
  float theta = parametric.x;
  float phi = parametric.y;

  vec4 xyz = Vec4((R + r*cos(phi))*cos(theta),
                  (R + r*cos(phi))*sin(theta),
                  r*sin(phi),
                  1
                 );
  gl_Position = gl_ModelViewProjectionMatrix * xyz;  // XXX fix me
  
  //eyeDirection = vec3(0);  // XXX fix me
  eyeDirection = normalize(eyePosition - gl_Position);

  //lightDirection = vec3(0);  // XXX fix me
  lightDirection = normalize(lightPosition - gl_Position);
 
  halfAngle = vec3(0);  // XXX fix me
  c0 = vec3(0);  // XXX fix me
  c1 = vec3(0);  // XXX fix me
  c2 = vec3(0);  // XXX fix me
}

