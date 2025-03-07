#pragma once

using namespace std;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

class Shader
{
public:
    GLuint Program;
    
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
        string vertexCode, geometryCode, fragmentCode;
        ifstream vShaderFile, gShaderFile, fShaderFile;

        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

        try
        {
            vShaderFile.open(vertexPath);
            gShaderFile.open(geometryPath);
            fShaderFile.open(fragmentPath);

            stringstream vShaderStream, gShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            gShaderStream << gShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            gShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            geometryCode = gShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (ifstream::failure e)
        {
            cout << "ERROR reading vertex or fragment file" << endl;
        }

        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* gShaderCode = geometryCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

        // Build and compile our shader program
        // Vertex shader
        GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates an empty shader object and returns a non-zero value by which it can be referenced
        glShaderSource(vertexShader, 1, &vShaderCode, NULL); // Sets the source code in shader to the source code in the array of strings specified by string. Any source code previously stored in the shader object is completely replaced
        glCompileShader(vertexShader); // Compiles the source code strings that have been stored in the shader object

        checkCompileErrors(vertexShader, "VERTEX");
        
        // Geometry shader
        GLint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &gShaderCode, NULL);
        glCompileShader(geometryShader);

        checkCompileErrors(geometryShader, "GEOMETRY");
        
        // Fragment shader
        GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);

        checkCompileErrors(fragmentShader, "FRAGMENT");

        // Link shaders
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertexShader);
        glAttachShader(this->Program, fragmentShader);
        glAttachShader(this->Program, geometryShader);
        glLinkProgram(this->Program);

        checkCompileErrors(this->Program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(geometryShader);

        Use();
        
        GLint texLocation = glGetUniformLocation(this->Program, "tex");

        if(texLocation != - 1)
        {
            glUniform1i(texLocation, 0);
        }
    }

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
        string vertexCode, fragmentCode;
        ifstream vShaderFile, fShaderFile;

        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);

            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (ifstream::failure e)
        {
            cout << "ERROR reading vertex or fragment file" << endl;
        }

        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

        // Build and compile our shader program
        // Vertex shader
        GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates an empty shader object and returns a non-zero value by which it can be referenced
        glShaderSource(vertexShader, 1, &vShaderCode, NULL); // Sets the source code in shader to the source code in the array of strings specified by string. Any source code previously stored in the shader object is completely replaced
        glCompileShader(vertexShader); // Compiles the source code strings that have been stored in the shader object

        checkCompileErrors(vertexShader, "VERTEX");
        
        // Fragment shader
        GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);

        checkCompileErrors(fragmentShader, "FRAGMENT");

        // Link shaders
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertexShader);
        glAttachShader(this->Program, fragmentShader);
        glLinkProgram(this->Program);

        checkCompileErrors(this->Program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        Use();
        
        GLint texLocation = glGetUniformLocation(this->Program, "tex");

        if(texLocation != - 1)
        {
            glUniform1i(texLocation, 0);
        }
    }

    ~Shader()
    {
        if(!Program) return;
        
        glDeleteProgram(this->Program);
        Program = 0;
    }
    
    void SelectSubroutine(string name) {
        // we search inside the Shader Program the name of the subroutine currently selected, and we get the numerical index
        GLuint index = glGetSubroutineIndex(this->Program, GL_FRAGMENT_SHADER, name.c_str());
        // we activate the subroutine using the index (this is where shaders swapping happens)
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index);
    }

    void SetFloat(string name, float value)
    {
        Use();
        GLint location = glGetUniformLocation(this->Program, name.c_str());
        if(location == -1) return;
        glUniform1f(location, value);
    }

    void SetInt(string name, int value)
    {
        Use();
        GLint location = glGetUniformLocation(this->Program, name.c_str());
        if(location == -1) return;
        glUniform1i(location, value);
    }

    void SetVec3(string name, int count, const float* value)
    {
        Use();
        GLint location = glGetUniformLocation(this->Program, name.c_str());
        if(location == -1) return;
        glUniform3fv(location, count, value);
    }

    void SetVec2(string name, int count, const float* value)
    {
        Use();
        GLint location = glGetUniformLocation(this->Program, name.c_str());
        if(location == -1) return;
        glUniform2fv(location, count, value);
    }

    void Use(string subroutine = string())
    {
        glUseProgram(this->Program); // Installs the program object specified by program as part of current rendering state
        if(!subroutine.empty())
        {
            //SelectSubroutine(subroutine);
        }
    }

    void Delete()
    {
        glDeleteProgram(this->Program);
        Program = 0;
    }

private:
    void checkCompileErrors(GLuint shader, string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if(type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << endl;
			}
		}
	}
};