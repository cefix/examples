
/*
 * Reimplement fixed function pipeline of OpenGL
 * So bypass all the data.
 * Precompute some constants which are needed for the convolution computation
 */


/**
 * Reimplement fixed pipeline
 **/
void main(void)
{
	// bypass the texture coordinate data
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// compute position of the pixel 
	gl_Position = ftransform();
	
	// bypass color data
	gl_FrontColor = gl_Color;
}

