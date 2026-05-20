#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
out vec3 vColor;
void main(){ vColor=aColor; gl_Position = vec4(aPos,1.0); }
#type fragment
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main(){ FragColor = vec4(vColor,1.0); }
