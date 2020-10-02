uniform sampler2D u_texture;

smooth in vec2 vf_UV;
smooth in vec4 vf_color;

out vec4 FragColor;


void main()
{
	vec2 tc = vf_UV;	 //	tc.y = 1.0 - vf_UV.y;
	
	// add mod color
	// vec4 modColor = 

	FragColor = texture(u_texture, tc);
}