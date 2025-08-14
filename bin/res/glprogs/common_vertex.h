layout (location = 0) in vec3 xyz; 
layout (location = 1) in vec3 normal; 
layout (location = 2) in vec3 tangent; 



#ifdef USING_BONES
layout (location = 3) in ivec4 bones; 	
#elif  defined(USING_PARTID)
layout (location = 3) in int partId; 	
#else
layout (location = 3) in vec4 color; 
#endif

layout (location = 4) in vec3 uv; 

uniform vec3 u_Scale;
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_TransformMatrix;



vec3 pullRightVector(mat4 invertedModelViewMatrix)
{
	return vec3(invertedModelViewMatrix[0][0],
		invertedModelViewMatrix[0][1],
		invertedModelViewMatrix[0][2]);
}


vec3 pullUpVector(mat4 invertedModelViewMatrix)
{
	return vec3(invertedModelViewMatrix[1][0],
		invertedModelViewMatrix[1][1],
		invertedModelViewMatrix[1][2]);
}

// Common transform routine
//
vec4 _transform()
{
	return u_ProjectionMatrix * (u_ModelViewMatrix * (u_TransformMatrix * vec4(xyz * u_Scale, 1.0)));
}

// Special case transform, for uses when vertex need extra transformation before transforming into screen space
//
vec4 _transform(vec3 pos)
{
	return u_ProjectionMatrix * (u_ModelViewMatrix * (u_TransformMatrix * vec4(pos, 1.0)));
}


