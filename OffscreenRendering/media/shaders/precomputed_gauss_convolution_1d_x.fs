/*
 * Apply convolution of variable size onto the pixels.
 * The convolution is done in 1D
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
uniform sampler2DRect color_buffer;
uniform sampler1D gaussian_kernel;

//! Size of the gaussian kernel (size = radius * 2)
uniform float radius;



/**
 **/
void main(void)
{
	// store here resulting color
	vec4 color = vec4(0.0);
	float totalWeigth = 0.0;
		// convolve by applying nsamples-time the texture lookup
	for (float i=-radius; i < radius; i += 1.0) 
	{
		// compute weight for the pixel 
		float weight = texture1D(gaussian_kernel, 0.5 + (i/radius*0.5)).r;
		totalWeigth += weight;
		
		// combine now the sum as all values multiplied by the weight
		color += texture2DRect(color_buffer, gl_TexCoord[0].xy +  vec2(i, 0) ) * weight;
	}
	color /= totalWeigth;
	
	gl_FragColor = color;
}
