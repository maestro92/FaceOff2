
##################################################################################
############################### Render Pipeline ##################################
##################################################################################

-	#, Stringizing operator 
this converts the argument you passed in into a string.

for example 

		#define stringer( x )    cout << #x << endl;

		stringer( abcdefg )

the preprocessor produces 

		cout << "abcdefg" << endl;

as mentioned above, it converts what you passed in into strings.



-	##, Token-pastin operator




				SDLGetOpenGLFunction(glBufferData);
	------->	SDLGetOpenGLFunction(glActiveTexture);
				SDLGetOpenGLFunction(glGetStringi);



I got an error saying "glActiveTexture": redefinition; previous definition was 'function'.

I assume the error is because glActiveTexture is not an OpenGL extension function. It already
exists in OpenGL 1.1. Hence we didnt need to GetProcAddress it. 

same thing for 

				OpenGLGlobalFunction(glTexImage3D)
				OpenGLGlobalFunction(glTexSubImage3D)

