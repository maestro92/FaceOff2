uniform sampler2D u_texture;

smooth in vec3 vf_normal;
smooth in vec2 vf_UV;
smooth in vec4 vf_color; //;

out vec4 FragColor;


void main()
{
	vec2 tc = vf_UV;	 //	tc.y = 1.0 - vf_UV.y;
	
	// add mod color
	// vec4 modColor = 
	FragColor = vec4(vf_normal.x, vf_UV.y, 0, 1);

	FragColor = vf_color; // texture(u_texture, tc);
	FragColor = vec4(1, 1, 0, 1);
}