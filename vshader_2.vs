#version 120
attribute vec3 vVertex;
attribute vec3 vColor;
attribute vec3 vNormal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

uniform vec3 vLightPosition[2];
uniform vec3 vLightColor[2];

varying vec3 fColor;
varying vec3 fLightColor[2];
varying vec3 fLightPositionCameraSpace[2];
varying vec3 fNormalCameraSpace;
varying vec3 fVertexPosition;
varying float fVertexLightDistance[2];
varying float fShadingFlag;

// attribute float vKs;
// attribute float vKa;
// attribute float vKd;

// uniform float fKs;
// uniform float fKa;
// uniform float fKd;

void main(void) {

	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	fShadingFlag = 1.0;
	fVertexPosition = (vView * vModel * vec4(vVertex, 1.0)).xyz;

	fNormalCameraSpace = (vView * vModel * vec4(vNormal, 0.0)).xyz;	// .xyz to convert to 3d vector
	fNormalCameraSpace = normalize(fNormalCameraSpace);

	for (int src = 0; src < 2; src++) {	
		fLightPositionCameraSpace[src] = (vView * vec4(vLightPosition[src], 1.0)).xyz + vec3(10.0, 10.0, 10.0) - (vView * vModel * vec4(vVertex, 1.0)).xyz;
		fLightPositionCameraSpace[src] = normalize(fLightPositionCameraSpace[src]);
		fLightColor[src] = vLightColor[src];
		fVertexLightDistance[src] = length(fVertexPosition - vLightPosition[src]);
	}
	// fKa = vKa;
	// fKs = vKs;
	// fKd = vKd;
	fColor = vColor;
}
