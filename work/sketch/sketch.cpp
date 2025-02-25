#include <string>

// Loader estensions OpenGL
// http://glad.dav1d.de/
// THIS IS OPTIONAL AND NOT REQUIRED, ONLY USE THIS IF YOU DON'T WANT GLAD TO INCLUDE windows.h
// GLAD will include windows.h for APIENTRY if it was not previously defined.
// Make sure you have the correct definition for APIENTRY for platforms which define _WIN32 but don't use __stdcall
#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>

// GLFW library to create window and to manage I/O
#include <glfw/glfw3.h>

// another check related to OpenGL loader
// confirm that GLAD didn't include windows.h
#ifdef _WINDOWS_
    #error windows.h was included!
#endif

// classes developed during lab lectures to manage shaders and to load models
#include <utils/shader.h>
#include <utils/texture.h>
#include <utils/texturecubemap.h>
#include <utils/model.h>
#include <utils/camera.h>

#include <utils/modelobject.h>
#include <utils/planeobject.h>
#include <utils/screenquadobject.h>
#include <utils/scene.h>

// we load the GLM classes used in the application
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>

// dimensions of application's window
GLuint screenWidth = 1920, screenHeight = 1080;
GLfloat fieldOfViewY = 45.0f;

// callback function for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void apply_camera_movements();
bool canMoveCamera = false;

// index of the current shader subroutine (= 0 in the beginning)
GLuint current_subroutine = 0;
// a vector for all the shader subroutines names used and swapped in the application
vector<std::string> subroutines;

// the name of the subroutines are searched in the shaders, and placed in the shaders vector (to allow shaders swapping)
void SetupShader(int shader_program);

// print on console the name of current shader subroutine
void PrintCurrentShader(int subroutine);

bool keys[1024];
GLfloat lastX, lastY;
bool firstMouse = true;

// parameters for time computation
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// rotation angle on Y axis
GLfloat orientationY = 0.0f;
// rotation speed on Y axis
GLfloat spin_speed = 15.0f;
// boolean to start/stop animated rotation on Y angle
GLboolean spinning = GL_FALSE;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

Camera camera(glm::vec3(0.0f, 0.0f, 7.0f), GL_TRUE);

glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);

GLfloat Kd = 0.5f;
GLfloat Ks = 0.4f;
GLfloat Ka = 0.1f;
GLfloat shininess = 25.0f;
GLfloat alpha = 0.2f;
GLfloat F0 = 0.9f;

glm::vec3 backgroundColor = glm::vec3(1.0f);
glm::vec3 edgeColor = glm::vec3(0.25f);
float colorSaturation = 0.4f;
float colorBrightness = 0.9f;
float edgeThreshold = 0.005f;
float noiseFrequencyEdge = 40.0f;
float noiseStrengthEdge = 0.00175f;
float noiseStrengthColor = 0.0025f;

void setup_illum_shader(Shader&);

/////////////////// MAIN function ///////////////////////
int main()
{
    // Initialization of OpenGL context using GLFW
    glfwInit();
    // We set OpenGL specifications required for this application
    // In this case: 4.1 Core
    // If not supported by your graphics HW, the context will not be created and the application will close
    // N.B.) creating GLAD code to load extensions, try to take into account the specifications and any extensions you want to use,
    // in relation also to the values indicated in these GLFW commands
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // we set if the window is resizable
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // we create the application's window
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Sketch", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // GLAD tries to load the context set by GLFW
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // we define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
	
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // we create the Shader Program used for objects (which presents different subroutines we can switch)
    Shader illum_shader = Shader("illumination_model.vert", "illumination_model.frag");
    Shader color_shader = Shader("basic.vert", "fullcolor.frag");
    Shader depth_shader = Shader("basic.vert", "depth.frag");
    Shader normal_shader = Shader("illumination_model.vert", "normal.frag");
    Shader cubemap_shader = Shader("cubemap.vert", "cubemap.frag");
    Shader screen_shader = Shader("screen.vert", "screen.frag");

    Texture uvTexture("../../textures/UV_Grid_Sm.png");
    Texture paperTexture("../../textures/paper_3000.jpg");
    paperTexture.setWrapS(GL_REPEAT);
    paperTexture.setWrapT(GL_REPEAT);
    Texture hatch_texture("../../textures/hatch_rgb.png");
    hatch_texture.setWrapS(GL_REPEAT);
    hatch_texture.setWrapT(GL_REPEAT);
    
    //TextureCubeMap hatch_texture_cubemap("../../textures/hatch_cubemap_hard/", ".jpg");
    // TextureCubeMap hatch_texture_cubemap("../../textures/cube/NissiBeach/", ".jpg");

    // we load the model(s) (code of Model class is in include/utils/model.h)
    ModelObject bunnyObject("Bunny", "../../models/bunny_lp.obj", illum_shader, glm::vec3(0.0f, 1.0f, -5.0f), 0.5f);
    bunnyObject.setTexture(&uvTexture);

    // we load the model(s) (code of Model class is in include/utils/model.h)
    ModelObject sphereObject("Sphere", "../../models/sphere.obj", illum_shader, glm::vec3(5.0f, 1.0f, -5.0f), 1.5f);

    ModelObject cubeObject("Cube", "../../models/cube.obj", illum_shader, glm::vec3(-5.0f, 1.0f, -5.0f), 1.5f);
    cubeObject.setColor(glm::vec3(0.0f, 0.0f, 1.0f));

    ModelObject cubeMapObject("CubeMap", "../../models/sphere.obj", cubemap_shader);
    cubeMapObject.setTexture(&hatch_texture);

    ModelObject floorObject("Floor", "../../models/plane.obj", illum_shader, glm::vec3(0.0f, -1.0f, 0.0f));
    floorObject.setScale(glm::vec3(10.0f, 1.0f, 10.0f));
    floorObject.setColor(glm::vec3(0.0f, 0.5f, 0.0f));

    ScreenQuadObject screen_quad;

    Scene mainScene;
    mainScene.add_object(&floorObject);
    mainScene.add_object(&bunnyObject);
    mainScene.add_object(&cubeObject);
    mainScene.add_object(&sphereObject);

    glm::mat4 view = glm::mat4(1.0f);

    // RBO
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // NORMAL FBO
    unsigned int normal_fbo;
    glGenFramebuffers(1, &normal_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, normal_fbo);

    unsigned int normal_fbo_texture;
    glGenTextures(1, &normal_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, normal_fbo_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normal_fbo_texture, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // DEPTH FBO
    unsigned int depth_fbo;
    glGenFramebuffers(1, &depth_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

    unsigned int depth_fbo_texture;
    glGenTextures(1, &depth_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, depth_fbo_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depth_fbo_texture, 0);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // CUBAMAP FBO
    unsigned int cubemap_fbo;
    glGenFramebuffers(1, &cubemap_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, cubemap_fbo);

    unsigned int cubemap_fbo_texture;
    glGenTextures(1, &cubemap_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, cubemap_fbo_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cubemap_fbo_texture, 0);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // SCREEN FBO
    unsigned int screen_fbo;
    glGenFramebuffers(1, &screen_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

    unsigned int screen_fbo_texture;
    glGenTextures(1, &screen_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, screen_fbo_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_fbo_texture, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR: Framebuffer is not complete" << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check is an I/O event is happening
        glfwPollEvents();
		
		//ImGUI new frame
		{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        }

        // ImGUI Show a simple window
        {
            static float f = 0.0f;
			GLfloat fieldOfViewX = glm::degrees(glm::atan(glm::tan(glm::radians(fieldOfViewY)/2) * screenWidth / screenHeight) * 2);

            ImGui::Begin("Parameters");
			
            ImGui::SeparatorText("FoV");
			if(ImGui::SliderFloat("FoV X", &fieldOfViewX, 0.0f, 180.0f)) {
				fieldOfViewY = glm::degrees(glm::atan(glm::tan(glm::radians(fieldOfViewX) / 2) * screenHeight / screenWidth) * 2);
			}
			ImGui::SliderFloat("FoV Y", &fieldOfViewY, 0.0f, 180.0f);
			
            ImGui::SeparatorText("Properties");
            ImGui::Checkbox("Wireframe", (bool*)&wireframe);
            ImGui::Checkbox("Spinning", (bool*)&spinning);
            
            if (ImGui::CollapsingHeader("Objects")) {
                for(auto i = mainScene.cbegin(); i != mainScene.cend(); ++i) {
                    (*i)->drawImGui();
                }
            }
            
			if(ImGui::CollapsingHeader("Sketch Configuration"))
			{
                ImGui::SeparatorText("Colors");
                ImGui::ColorEdit3("Background Color", (float*)&backgroundColor);
                ImGui::ColorEdit3("Edge Color", (float*)&edgeColor);
                ImGui::SliderFloat("Color saturation", &colorSaturation, 0.0f, 1.0f);
                ImGui::SliderFloat("Color brightness", &colorBrightness, 0.0f, 1.0f);
                ImGui::SeparatorText("Edge");
                ImGui::SliderFloat("Edge threshold", &edgeThreshold, 0.0f, 2.0f, "%.3f");
                ImGui::SeparatorText("Noise");
                ImGui::SliderFloat("Noise frequency edge", &noiseFrequencyEdge, 0.0f, 500.0f);
                ImGui::SliderFloat("Noise strength edge", &noiseStrengthEdge, 0.0f, 0.005f, "%.5f");
                ImGui::SliderFloat("Noise strength color", &noiseStrengthColor, 0.0f, 0.005f, "%.5f");
            }

			if(ImGui::CollapsingHeader("Illumination Model Configuration"))
			{
				ImGui::InputFloat3("Light Position", (float*)&lightPos0);
                ImGui::SeparatorText("Illumination model parameters");
				ImGui::SliderFloat("Kd", (float*)&Kd, 0.0f, 2.0f);
				ImGui::SliderFloat("Ks", (float*)&Ks, 0.0f, 2.0f);
				ImGui::SliderFloat("Ka", (float*)&Ka, 0.0f, 5.0f);
				ImGui::SliderFloat("Shininess", (float*)&shininess, 0.0f, 100.0f);
				ImGui::SliderFloat("Alpha", (float*)&alpha, 0.0f, 5.0f);
				ImGui::SliderFloat("F0", (float*)&F0, 0.0f, 100.0f);
			}

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }
		
        glm::mat4 projection = glm::perspective(
                                    glm::radians(fieldOfViewY),
                                    (float)screenWidth/(float)screenHeight,
                                    0.1f,
                                    10000.0f
                                );
        
        apply_camera_movements();
        
        view = camera.GetViewMatrix();

        ///////////////////

        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        if (spinning)
        {
            orientationY+=(deltaTime*spin_speed);

            glm::vec3 rotation = bunnyObject.rotation();
            rotation.y = orientationY;
            bunnyObject.setRotation(rotation);

            rotation = cubeObject.rotation();
            rotation.y = orientationY;
            cubeObject.setRotation(rotation);

            rotation = sphereObject.rotation();
            rotation.y = orientationY;
            sphereObject.setRotation(rotation);
        }

        setup_illum_shader(illum_shader);
        normal_shader.Use();
        normal_shader.SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));
        
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // RENDER ON NORMAL FBO
        glBindFramebuffer(GL_FRAMEBUFFER, normal_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainScene.draw(view, projection, &normal_shader);

        // RENDER ON DEPTH FBO
        glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mainScene.draw(view, projection, &depth_shader);
        
        // RENDER ON CUBEMAP FBO
        glBindFramebuffer(GL_FRAMEBUFFER, cubemap_fbo);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // cubemap_shader.SetInt("tCube", 0);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, hatch_texture_cubemap.name());
        
        glDepthFunc(GL_LEQUAL);
        glm::mat4 cubemapViewMatrix = glm::mat4(
            camera.WorldFront.x, 0.0, - camera.WorldFront.z, 0.0,
            0.0, 1.0, 0.0, 0.0,
            camera.WorldFront.z, 0.0, camera.WorldFront.x, 0.0,
            0.0, 0.0, 0.0, 1.0
        );
        cubemap_shader.SetFloat("viewAngleY", glm::asin(camera.Front.y));
        cubeMapObject.draw(cubemapViewMatrix, projection);
        glDepthFunc(GL_LESS);
        
        // RENDER ON SCREEN FBO
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainScene.draw(view, projection);
        
        
        // RENDER ON DEFAULT FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_DEPTH_TEST);

        screen_shader.SetInt("normalTexture", 0);
        screen_shader.SetInt("depthTexture", 1);
        screen_shader.SetInt("screenTexture", 2);
        screen_shader.SetInt("hatchTexture", 3);
        screen_shader.SetInt("paperTexture", 4);
        
        screen_shader.SetVec3("background_color", 1, glm::value_ptr(backgroundColor));
        screen_shader.SetVec3("edge_color", 1, glm::value_ptr(edgeColor));
        screen_shader.SetFloat("color_saturation", colorSaturation);
        screen_shader.SetFloat("color_brightness", colorBrightness);
        screen_shader.SetFloat("edge_threshold", edgeThreshold);
        screen_shader.SetFloat("noise_frequency_edge", noiseFrequencyEdge);
        screen_shader.SetFloat("noise_strength_edge", noiseStrengthEdge);
        screen_shader.SetFloat("noise_strength_color", noiseStrengthColor);
        
        screen_shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normal_fbo_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_fbo_texture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, screen_fbo_texture);
        // glActiveTexture(GL_TEXTURE3);
        // glBindTexture(GL_TEXTURE_2D, hatch_texture.name());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, cubemap_fbo_texture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, paperTexture.name());

        screen_quad.draw();
        
        // Rendering imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &normal_fbo);
    glDeleteFramebuffers(1, &depth_fbo);
    glDeleteFramebuffers(1, &cubemap_fbo);
    glDeleteFramebuffers(1, &screen_fbo);

    // when I exit from the graphics loop, it is because the application is closing
    // we delete the Shader Programs
    color_shader.Delete();
    depth_shader.Delete();
    illum_shader.Delete();
    screen_shader.Delete();

    // we close and delete the created context
    glfwTerminate();

    return 0;
}


//////////////////////////////////////////
// The function parses the content of the Shader Program, searches for the Subroutine type names,
// the subroutines implemented for each type, print the names of the subroutines on the terminal, and add the names of
// the subroutines to the shaders vector, which is used for the shaders swapping
void SetupShader(int program)
{
    int maxSub,maxSubU,countActiveSU;
    GLchar name[256];
    int len, numCompS;

    // global parameters about the Subroutines parameters of the system
    glGetIntegerv(GL_MAX_SUBROUTINES, &maxSub);
    glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubU);
    std::cout << "Max Subroutines:" << maxSub << " - Max Subroutine Uniforms:" << maxSubU << std::endl;

    // get the number of Subroutine uniforms (only for the Fragment shader, due to the nature of the exercise)
    // it is possible to add similar calls also for the Vertex shader
    glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);

    // print info for every Subroutine uniform
    for (int i = 0; i < countActiveSU; i++) {

        // get the name of the Subroutine uniform (in this example, we have only one)
        glGetActiveSubroutineUniformName(program, GL_FRAGMENT_SHADER, i, 256, &len, name);
        // print index and name of the Subroutine uniform
        std::cout << "Subroutine Uniform: " << i << " - name: " << name << std::endl;

        // get the number of subroutines
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

        // get the indices of the active subroutines info and write into the array s
        int *s =  new int[numCompS];
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_COMPATIBLE_SUBROUTINES, s);
        std::cout << "Compatible Subroutines:" << std::endl;

        // for each index, get the name of the subroutines, print info, and save the name in the shaders vector
        for (int j=0; j < numCompS; ++j) {
            glGetActiveSubroutineName(program, GL_FRAGMENT_SHADER, s[j], 256, &len, name);
            std::cout << "\t" << s[j] << " - " << name << "\n";
            subroutines.push_back(name);
        }
        std::cout << std::endl;

        delete[] s;
    }
}

//////////////////////////////////////////
// we print on console the name of the currently used shader subroutine
void PrintCurrentShader(int subroutine)
{
    std::cout << "Current shader subroutine: " << subroutines[subroutine]  << std::endl;
}

//////////////////////////////////////////
// callback for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  GLuint new_subroutine;

  // if ESC is pressed, we close the application
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

  // if P is pressed, we start/stop the animated rotation of models
  if(key == GLFW_KEY_P && action == GLFW_PRESS)
      spinning=!spinning;

  // if L is pressed, we activate/deactivate wireframe rendering of models
  if(key == GLFW_KEY_L && action == GLFW_PRESS)
      wireframe=!wireframe;

    // pressing a key number, we change the shader applied to the models
    // if the key is between 1 and 9, we proceed and check if the pressed key corresponds to
    // a valid subroutine
    if((key >= GLFW_KEY_1 && key <= GLFW_KEY_9) && action == GLFW_PRESS)
    {
        // "1" to "9" -> ASCII codes from 49 to 59
        // we subtract 48 (= ASCII CODE of "0") to have integers from 1 to 9
        // we subtract 1 to have indices from 0 to 8
        new_subroutine = (key-'0'-1);
        // if the new index is valid ( = there is a subroutine with that index in the shaders vector),
        // we change the value of the current_subroutine variable
        // NB: we can just check if the new index is in the range between 0 and the size of the shaders vector,
        // avoiding to use the std::find function on the vector
        if (new_subroutine<subroutines.size())
        {
            current_subroutine = new_subroutine;
            PrintCurrentShader(current_subroutine);
        }
    }

    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        canMoveCamera = true;
        firstMouse = true;
    }
    else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        canMoveCamera = false;
    }

    if(action == GLFW_PRESS)
    {
        keys[key] = true;
    }
    else if(action == GLFW_RELEASE)
    {
        keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(!canMoveCamera) return;

    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void apply_camera_movements()
{
    GLboolean diagonal_movement = (keys[GLFW_KEY_W] ^ keys[GLFW_KEY_S]) && (keys[GLFW_KEY_A] ^ keys[GLFW_KEY_D]);
    camera.SetMovementCompensation(diagonal_movement);
    
    if(keys[GLFW_KEY_W])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(keys[GLFW_KEY_S])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(keys[GLFW_KEY_A])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(keys[GLFW_KEY_D])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        canMoveCamera = true;
        firstMouse = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        canMoveCamera = false;
    }
}

void setup_illum_shader(Shader& illum_shader)
{
    illum_shader.Use();
    GLint pointLightLocation = glGetUniformLocation(illum_shader.Program, "pointLightPosition");
    GLint matSpecularLocation = glGetUniformLocation(illum_shader.Program, "specularColor");
    GLint matAmbientLocation = glGetUniformLocation(illum_shader.Program, "ambientColor");
    GLint kdLocation = glGetUniformLocation(illum_shader.Program, "Kd");
    GLint ksLocation = glGetUniformLocation(illum_shader.Program, "Ks");
    GLint kaLocation = glGetUniformLocation(illum_shader.Program, "Ka");
    GLint shininessLocation = glGetUniformLocation(illum_shader.Program, "shininess");
    GLint alphaLocation = glGetUniformLocation(illum_shader.Program, "alpha");
    GLint f0Location = glGetUniformLocation(illum_shader.Program, "F0");
    
    glUniform3fv(pointLightLocation, 1, glm::value_ptr(lightPos0));
    glUniform1f(kdLocation, Kd);
    glUniform1f(ksLocation, Ks);
    glUniform1f(kaLocation, Ka);
    glUniform1f(shininessLocation, shininess);
    glUniform1f(alphaLocation, alpha);
    glUniform1f(f0Location, F0);
}