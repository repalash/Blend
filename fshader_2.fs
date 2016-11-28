#version 120

varying vec3 fColor;
varying vec3 fLightColor[2];
varying vec3 fLightPositionCameraSpace[2];
varying vec3 fNormalCameraSpace;
varying vec3 fVertexPosition;
varying float fVertexLightDistance[2];
varying float fShadingFlag;

void main(void) {
	if (fShadingFlag == 1.0) {					// Phong shading
		vec3 frag_color = vec3(0, 0, 0);
		for (int src = 0; src < 2; src++) {	
			float normal_light_theta = max(0, dot(normalize(fNormalCameraSpace), normalize(fLightPositionCameraSpace[src])));
			vec3 diffuseComp = fColor * fLightColor[src] * normal_light_theta;
			
			vec3 halfBis = normalize(-fVertexPosition) + normalize(fLightPositionCameraSpace[src]);
			halfBis = normalize(halfBis);

			float normal_half_theta = max(0, dot(halfBis, normalize(fNormalCameraSpace)));
			vec3 specularComp = vec3(0.0, 0.0, 0.0) * fLightColor[src] * pow(normal_half_theta, 256);

			float fAtt = 0.0017 * fVertexLightDistance[src] * fVertexLightDistance[src];
			
			vec3 result_color = vec3(0, 0, 0);
			result_color += (diffuseComp / fAtt);
			result_color += (specularComp / fAtt);
			
			frag_color += result_color;
		}
		vec3 ambientComp = vec3(0.2, 0.2, 0.2) * fColor * fLightColor[0];
		frag_color += ambientComp;
		gl_FragColor = vec4(frag_color, 1.0);
	}
	else {									
		gl_FragColor = vec4(fColor, 1.0);	
	}
}
