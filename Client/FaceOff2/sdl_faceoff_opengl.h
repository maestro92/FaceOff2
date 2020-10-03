#pragma once


#include "SDL_opengl.h"

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <fstream>      // std::ifstream


typedef void APIENTRY type_glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void APIENTRY type_glBindFramebuffer(GLenum target, GLuint framebuffer);
typedef void APIENTRY type_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
typedef void APIENTRY type_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum APIENTRY type_glCheckFramebufferStatus(GLenum target);
typedef void APIENTRY type_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void APIENTRY type_glAttachShader(GLuint program, GLuint shader);
typedef void APIENTRY type_glCompileShader(GLuint shader);
typedef GLuint APIENTRY type_glCreateProgram(void);
typedef GLuint APIENTRY type_glCreateShader(GLenum type);
typedef void APIENTRY type_glLinkProgram(GLuint program);
typedef void APIENTRY type_glShaderSource(GLuint shader, GLsizei count, GLchar **string, GLint *length);
typedef void APIENTRY type_glUseProgram(GLuint program);
typedef void APIENTRY type_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void APIENTRY type_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void APIENTRY type_glValidateProgram(GLuint program);
typedef void APIENTRY type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef GLint APIENTRY type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef void APIENTRY type_glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
typedef void APIENTRY type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void APIENTRY type_glUniform1i(GLint location, GLint v0);

typedef void APIENTRY type_glUniform1f(GLint location, GLfloat v0);
typedef void APIENTRY type_glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
typedef void APIENTRY type_glUniform3fv(GLint location, GLsizei count, const GLfloat *value);

typedef void APIENTRY type_glEnableVertexAttribArray(GLuint index);
typedef void APIENTRY type_glDisableVertexAttribArray(GLuint index);
typedef GLint APIENTRY type_glGetAttribLocation(GLuint program, const GLchar *name);
typedef void APIENTRY type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void APIENTRY type_glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void APIENTRY type_glBindVertexArray(GLuint array);
typedef void APIENTRY type_glGenVertexArrays(GLsizei n, GLuint *arrays);
typedef void APIENTRY type_glBindBuffer(GLenum target, GLuint buffer);
typedef void APIENTRY type_glGenBuffers(GLsizei n, GLuint *buffers);
typedef void APIENTRY type_glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void APIENTRY type_glActiveTexture(GLenum texture);
typedef void APIENTRY type_glDeleteProgram(GLuint program);
typedef void APIENTRY type_glDeleteShader(GLuint shader);
typedef void APIENTRY type_glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
typedef void APIENTRY type_glDrawBuffers(GLsizei n, const GLenum *bufs);
typedef void APIENTRY type_glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void APIENTRY type_glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);

typedef void APIENTRY type_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);

typedef void APIENTRY GL_DEBUG_PROC(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
typedef void APIENTRY type_glDebugMessageCallbackARB(GL_DEBUG_PROC* callback, const void* userParam);


typedef const GLubyte * APIENTRY type_glGetStringi(GLenum name, GLuint index);

#define OpenGLGlobalFunction(Name) static type_##Name *Name;

OpenGLGlobalFunction(glTexImage2DMultisample)
OpenGLGlobalFunction(glBindFramebuffer)
OpenGLGlobalFunction(glGenFramebuffers)
OpenGLGlobalFunction(glFramebufferTexture2D)
OpenGLGlobalFunction(glCheckFramebufferStatus)
OpenGLGlobalFunction(glBlitFramebuffer)
OpenGLGlobalFunction(glAttachShader)
OpenGLGlobalFunction(glCompileShader)
OpenGLGlobalFunction(glCreateProgram)
OpenGLGlobalFunction(glCreateShader)
OpenGLGlobalFunction(glLinkProgram)
OpenGLGlobalFunction(glShaderSource)
OpenGLGlobalFunction(glUseProgram)
OpenGLGlobalFunction(glGetProgramInfoLog)
OpenGLGlobalFunction(glGetShaderInfoLog)
OpenGLGlobalFunction(glValidateProgram)
OpenGLGlobalFunction(glGetProgramiv)
OpenGLGlobalFunction(glGetUniformLocation)
OpenGLGlobalFunction(glUniform4fv)
OpenGLGlobalFunction(glUniformMatrix4fv)
OpenGLGlobalFunction(glUniform1i)

OpenGLGlobalFunction(glUniform1f)
OpenGLGlobalFunction(glUniform2fv)
OpenGLGlobalFunction(glUniform3fv)

OpenGLGlobalFunction(glEnableVertexAttribArray)
OpenGLGlobalFunction(glDisableVertexAttribArray)
OpenGLGlobalFunction(glGetAttribLocation)
OpenGLGlobalFunction(glVertexAttribPointer)
OpenGLGlobalFunction(glVertexAttribIPointer)
OpenGLGlobalFunction(glDebugMessageCallbackARB);
OpenGLGlobalFunction(glBindVertexArray)
OpenGLGlobalFunction(glGenVertexArrays)
OpenGLGlobalFunction(glBindBuffer)
OpenGLGlobalFunction(glGenBuffers)
OpenGLGlobalFunction(glBufferData)
OpenGLGlobalFunction(glDeleteProgram)
OpenGLGlobalFunction(glDeleteShader)
OpenGLGlobalFunction(glDeleteFramebuffers)
OpenGLGlobalFunction(glDrawBuffers)

OpenGLGlobalFunction(glDrawElementsBaseVertex)

OpenGLGlobalFunction(glGetStringi);

static char* globalShaderHeaderCode;
static std::string shadersFolderPath = "./shaders/";

// SDL 
void SDLInitOpenGL(SDL_Window* window)
{
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Using 3.3 OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	// Create OpenGL Context
	SDL_GLContext openGLContext = SDL_GL_CreateContext(window);
	if (openGLContext)
	{
		SDL_GL_MakeCurrent(window, openGLContext);

		int ContextMajorVersion;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &ContextMajorVersion);
		bool isModernContext = ContextMajorVersion >= 3;

		// do all the proc address here

#define SDLGetOpenGLFunction(name) name = (type_##name *)SDL_GL_GetProcAddress(#name)

		SDLGetOpenGLFunction(glTexImage2DMultisample);
		SDLGetOpenGLFunction(glBindFramebuffer);
		SDLGetOpenGLFunction(glGenFramebuffers);
		SDLGetOpenGLFunction(glFramebufferTexture2D);
		SDLGetOpenGLFunction(glCheckFramebufferStatus);
		SDLGetOpenGLFunction(glBlitFramebuffer);
		SDLGetOpenGLFunction(glAttachShader);
		SDLGetOpenGLFunction(glCompileShader);
		SDLGetOpenGLFunction(glCreateProgram);
		SDLGetOpenGLFunction(glCreateShader);
		SDLGetOpenGLFunction(glLinkProgram);
		SDLGetOpenGLFunction(glShaderSource);
		SDLGetOpenGLFunction(glUseProgram);
		SDLGetOpenGLFunction(glGetProgramInfoLog);
		SDLGetOpenGLFunction(glGetShaderInfoLog);
		SDLGetOpenGLFunction(glValidateProgram);
		SDLGetOpenGLFunction(glGetProgramiv);
		SDLGetOpenGLFunction(glGetUniformLocation);
		SDLGetOpenGLFunction(glUniform4fv);
		SDLGetOpenGLFunction(glUniformMatrix4fv);
		SDLGetOpenGLFunction(glUniform1i);
		SDLGetOpenGLFunction(glEnableVertexAttribArray);
		SDLGetOpenGLFunction(glDisableVertexAttribArray);
		SDLGetOpenGLFunction(glGetAttribLocation);
		SDLGetOpenGLFunction(glVertexAttribPointer);
		SDLGetOpenGLFunction(glVertexAttribIPointer);
		SDLGetOpenGLFunction(glDebugMessageCallbackARB);
		SDLGetOpenGLFunction(glBindVertexArray);
		SDLGetOpenGLFunction(glGenVertexArrays);
		SDLGetOpenGLFunction(glBindBuffer);
		SDLGetOpenGLFunction(glGenBuffers);
		SDLGetOpenGLFunction(glBufferData);
		//	SDLGetOpenGLFunction(glActiveTexture);
		SDLGetOpenGLFunction(glGetStringi);
		SDLGetOpenGLFunction(glDeleteProgram);
		SDLGetOpenGLFunction(glDeleteShader);
		SDLGetOpenGLFunction(glDeleteFramebuffers);
		SDLGetOpenGLFunction(glDrawBuffers);
		//	SDLGetOpenGLFunction(glTexImage3D);
		//	SDLGetOpenGLFunction(glTexSubImage3D);
		SDLGetOpenGLFunction(glDrawElementsBaseVertex);
		SDLGetOpenGLFunction(glUniform1f);
		SDLGetOpenGLFunction(glUniform2fv);
		SDLGetOpenGLFunction(glUniform3fv);

		SDL_GL_SetSwapInterval(1);
	}

}



void loadFile(const char* filename, std::string & outputStr)
{
	std::string mstr(filename);

	std::ifstream in(mstr);

	if (!in.is_open())
	{
		std::cout << "The file " << filename << " cannot be openned\n" << std::endl;
		exit(1);
		return;
	}

	char tmp[512];
	while (!in.eof())
	{
		in.getline(tmp, 512);
		outputStr += tmp;
		outputStr += '\n';
	}
}


struct TexturedQuadlShader
{
	GLuint programHandle;

	GLuint MVPMatId;
	GLuint textureId;

	GLuint vertexPositionId;
	GLuint vertexNormalId;
	GLuint vertexUVId;
	GLuint vertexColorId;
};


struct OpenGLStuff
{
	GLuint vertexBufferHandle;
	TexturedQuadlShader generalShader;
};


void WINAPI OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	char* ErrorMessage = (char *)message;
	std::cout << ErrorMessage << std::endl;
	assert(!"OpenGL Error encountered");
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

GLuint OpenGLCreateShaderProgram(char* defines, char* headerCode, char* vertexCode, char* fragmentCode)
{
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLchar* vertexShaderCode[] = 
	{
		defines,
		headerCode,
		vertexCode,
	};
	glShaderSource(vertexShaderId, ArrayCount(vertexShaderCode), vertexShaderCode, 0);
	glCompileShader(vertexShaderId);

	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar* fragmentShaderCode[] =
	{
		defines,
		headerCode,
		fragmentCode,
	};
	glShaderSource(fragmentShaderId, ArrayCount(fragmentShaderCode), fragmentShaderCode, 0);
	glCompileShader(fragmentShaderId);
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);

	glValidateProgram(program);
	GLint linked = false;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei ignored;
		char vertexErrors[4096];
		char fragmentErrors[4096];
		char programErrors[4096];
		glGetShaderInfoLog(vertexShaderId, sizeof(vertexErrors), &ignored, vertexErrors);
		glGetShaderInfoLog(fragmentShaderId, sizeof(fragmentErrors), &ignored, fragmentErrors);
		glGetProgramInfoLog(program, sizeof(programErrors), &ignored, programErrors);

		if (strlen(vertexErrors) != 0)
		{
			std::cout << "Shader Compile Status: \n" << vertexErrors << std::endl << std::endl;
		}

		if (strlen(fragmentErrors) != 0)
		{
			std::cout << "Shader Compile Status: \n" << fragmentErrors << std::endl << std::endl;
		}
		assert(!"Shader validation failed");
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return program;
}

void CompileGeneralShaderProgram(TexturedQuadlShader* shaderProgram)
{
	std::cout << ">>>>> compiling general shader" << std::endl;

	char defines[1024] = "#version 130\n";
	
	// vertex shader
	std::string fileName = shadersFolderPath + "textured_bitmap.vs";
	std::string vertexCodeString;
	loadFile(fileName.c_str(), vertexCodeString);
	char vertexCode[4096];
	strcpy(vertexCode, vertexCodeString.c_str());

	// fragment shader
	fileName = shadersFolderPath + "textured_bitmap.fs";
	std::string fragmentCodeString;
	loadFile(fileName.c_str(), fragmentCodeString);
	char fragmentCode[4096];
	strcpy(fragmentCode, fragmentCodeString.c_str());

	GLuint programId = OpenGLCreateShaderProgram(defines, globalShaderHeaderCode, vertexCode, fragmentCode);
	
	shaderProgram->programHandle = programId;
	shaderProgram->MVPMatId = glGetUniformLocation(programId, "u_MVPMat");
	shaderProgram->textureId = glGetUniformLocation(programId, "u_texture");

	shaderProgram->vertexPositionId = glGetAttribLocation(programId, "position");
	shaderProgram->vertexNormalId = glGetAttribLocation(programId, "normal");
	shaderProgram->vertexUVId = glGetAttribLocation(programId, "UV");
	shaderProgram->vertexColorId = glGetAttribLocation(programId, "color");
}


bool IsValidArray(GLuint id)
{
	return id != -1;
}


void UseShaderProgramBegin(TexturedQuadlShader* program, glm::mat4* cameraTransform)
{
	glUseProgram(program->programHandle);

	GLuint positionArray = program->vertexPositionId;
	GLuint normalArray = program->vertexNormalId;
	GLuint UVArray = program->vertexUVId;
	GLuint colorArray = program->vertexColorId;

	if (IsValidArray(positionArray))
	{
		glEnableVertexAttribArray(positionArray);
		glVertexAttribPointer(positionArray, 3, GL_FLOAT, false, sizeof(TexturedVertex), (void*)OffsetOf(TexturedVertex, position));
	}
	
	if (IsValidArray(normalArray))
	{
		glEnableVertexAttribArray(normalArray);
		glVertexAttribPointer(normalArray, 3, GL_FLOAT, false, sizeof(TexturedVertex), (void*)OffsetOf(TexturedVertex, normal));
	}	
	
	if (IsValidArray(UVArray))
	{
		glEnableVertexAttribArray(UVArray);
		glVertexAttribPointer(UVArray, 2, GL_FLOAT, false, sizeof(TexturedVertex), (void*)OffsetOf(TexturedVertex, uv));
	}

	if (IsValidArray(colorArray))
	{
		glEnableVertexAttribArray(colorArray);
		glVertexAttribPointer(colorArray, 4, GL_FLOAT, false, sizeof(TexturedVertex), (void*)OffsetOf(TexturedVertex, color));
	}

	glUniformMatrix4fv(program->MVPMatId, 1, GL_FALSE, glm::value_ptr(*cameraTransform));
	glUniform1i(program->textureId, 0);
}

void UseShaderProgramEnd(TexturedQuadlShader* program)
{
	glUseProgram(0);

	GLuint positionArray = program->vertexPositionId;
	GLuint normalArray = program->vertexNormalId;
	GLuint UVArray = program->vertexUVId;
	GLuint colorArray = program->vertexColorId;

	if (IsValidArray(positionArray))
	{
		glDisableVertexAttribArray(positionArray);
	}

	if (IsValidArray(normalArray))
	{
		glDisableVertexAttribArray(normalArray);
	}

	if (IsValidArray(UVArray))
	{
		glDisableVertexAttribArray(UVArray);
	}

	if (IsValidArray(colorArray))
	{
		glDisableVertexAttribArray(colorArray);
	}

}


static int counter = 0;
glm::vec3 backgroundColor;

void OpenGLRenderCommands(OpenGLStuff* openGL, GameRenderCommands* commands, glm::ivec2 drawRegionMin,
	glm::ivec2 drawRegionMax,
	glm::ivec2 windowDimensions)
{
	windowDimensions.x = 800;
	windowDimensions.y = 640;
	int w = 800;
	int h = 640;


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowDimensions.x, windowDimensions.y);
	
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glEnableVertexAttribArray(openGL->vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, openGL->vertexBufferHandle);
	glVertexAttribPointer(openGL->vertexBufferHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, 1);

	glDisableVertexAttribArray(0);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	/*
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		// Process/log the error.
		std::cout << "err " << err << std::endl;
	}
	*/

#if 0
	uint8* curAt = commands->pushBufferBase;
	UseShaderProgramBegin(&openGL->generalShader, &commands->cameraTransform);

	// gl buffer data
	glBufferData(GL_ARRAY_BUFFER, commands->numVertex * sizeof(TexturedVertex),
		commands->masterVertexArray, GL_STREAM_DRAW);



	for (int i = 0; i < commands->numElements; i++)
	{
		RenderEntryHeader* header = (RenderEntryHeader*)curAt;
		curAt += sizeof(RenderEntryHeader);

		void* data = (uint8*)header + sizeof(*header);
		switch (header->type)
		{
			case RenderEntryType_Clear:
			{

			}
			break;
			
			case RenderEntryType_TexturedQuads:
			{	

				// Iterate 
				curAt += sizeof(RenderEntryTexturedQuads);
				RenderEntryTexturedQuads* entry = (RenderEntryTexturedQuads*)data;


				std::cout << "drawing " << entry->numQuads << std::endl;

				for (int i = 0; i < entry->numQuads; i++)
				{
					int offset = entry->masterVertexArrayOffset + i * 4;
					glDrawArrays(GL_POINTS, offset, 4);
				}

			}
			break;

			case RenderEntryType_CoordinateSystem:
			{

			}
			break;
		}
	}
	UseShaderProgramEnd(&openGL->generalShader);
#endif
}




void initApplicationOpenGL(OpenGLStuff* openGL)
{
	globalShaderHeaderCode = new char[4096];
	std::string globalShaderHeaderCodeString;
	strcpy(globalShaderHeaderCode, globalShaderHeaderCodeString.c_str());


	GLuint DummyVertexArray;
	glGenVertexArrays(1, &DummyVertexArray);
	glBindVertexArray(DummyVertexArray);



	glm::vec3 vertices[1];
	vertices[0] = glm::vec3(0.0f, 0.0f, 0.0f);

	glGenBuffers(1, &openGL->vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, openGL->vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

	CompileGeneralShaderProgram(&openGL->generalShader);


	if (glDebugMessageCallbackARB)
	{
		// glEnable(GL_DEBUG_OUTPUT);
		// glDebugMessageCallbackARB(MessageCallback, 0);

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB(OpenGLDebugCallback, 0);
		
		
	}
	

}
