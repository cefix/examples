uniform sampler2DRect color_buffer;

// Colormap of the scene blurred
uniform sampler2DRect color_buffer_blurred;

// Colormap of the scene blurred
uniform sampler2DRect color_buffer_blurred_more;

// Depth map containing depth values of the image map
uniform sampler2DRect depth_buffer;

// Focal length of the camera
uniform float focal_length;

// Focal range of the camera 
uniform float focal_range;

// zNear and zFar values 
uniform float zNear;
uniform float zFar;

float convertZ( in float near, in float far, in float depthBufferValue )
{
        float clipZ = ( depthBufferValue - 0.5 ) * 2.0;
        return -(2.0 * far * near) / ( clipZ * ( far - near ) - ( far + near ));
}

/**
 **/
void main(void)
{
    vec2 inTex = gl_TexCoord[0].st;
    
    // compute distance to the viewer
    float a = zFar / ( zFar - zNear );
    float b = zFar * zNear / ( zNear - zFar );
    
    float depth = texture2DRect( depth_buffer, inTex ).x;
    float dist = b / ( depth - a );
    
    // get color map and blurred color map values 
    vec4 colorValue = texture2DRect (color_buffer, inTex).rgba;
    vec4 blurredValue1 = texture2DRect ( color_buffer_blurred, inTex / 2.0).rgba;
    vec4 blurredValue2 = texture2DRect ( color_buffer_blurred_more, inTex / 4.0).rgba;
    
    // now compute the bluriness value 
    float blur = clamp(abs(dist - focal_length) / focal_range, 0.0, 1.0);
    float factor1 = 1.0;
    float factor2 = 0.0;
    
    // compute blend factors 
    if (blur > 0.5)
        factor2 = (blur - 0.5) * 2.0;
    else
        factor1 = blur * 2.0;
        
	
    // the resulting color value is the combination of blurred and non-blurred map 
    vec4 result = mix(colorValue, blurredValue1, factor1);
    gl_FragColor = mix(result, blurredValue2, factor2);
    
}
