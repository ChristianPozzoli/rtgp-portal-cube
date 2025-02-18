/*
work10

Real-Time Graphics Programming - a.a. 2023/2024
Master degree in Computer Science
Universita' degli Studi di Milano
*/

/*
OpenGL coordinate system (right-handed)
positive X axis points right
positive Y axis points up
positive Z axis points "outside" the screen


                              Y
                              |
                              |
                              |________X
                             /
                            /
                           /
                          Z
*/

// Std. Includes
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
#include <utils/model.h>
#include <utils/camera.h>

#include <utils/modelobject.h>
#include <utils/planeobject.h>
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

glm::vec3 diffuseColor(1.0f, 0.0f, 0.0f);
glm::vec3 specularColor(1.0f, 1.0f, 1.0f);
glm::vec3 ambientColor(0.1f, 0.1f, 0.1f);

glm::vec3 leftFrontBorderColor(0.0f, 1.0f, 0.0f);
glm::vec3 rightFrontBorderColor(1.0f, 0.0f, 0.0f);
glm::vec3 leftBackBorderColor(1.0f, 1.0f, 0.0f);
glm::vec3 rightBackBorderColor(1.0f, 0.0f, 1.0f);

GLfloat Kd = 0.5f;
GLfloat Ks = 0.4f;
GLfloat Ka = 0.1f;
GLfloat shininess = 25.0f;
GLfloat alpha = 0.2f;
GLfloat F0 = 0.9f;

GLfloat celShadingThickness = .3f;

// color to be passed as uniform to the shader of the plane
glm::vec3 groundColor(0.0f, 0.5f, 0.0f);

GLfloat planeScale = 0.95f;
GLfloat planeBorder = 0.0f;

glm::vec3 bunnyPosition(0.0f, 1.0f, -5.0f);
GLfloat bunnyScale = 0.5f;

glm::vec3 cubeStructurePosition(0.0f, 1.0f, -3.0f);
glm::vec3 cubeStructureRotation(0.0f, 0.0f, 0.0f);
GLfloat cubeStructureScale = 3.5f;

glm::vec3 mainScenePosition(0.0f, 0.0f, 0.0f);
glm::vec3 mainSceneRotation(0.0f, 0.0f, 0.0f);
GLfloat mainSceneScale = 1.0f;

glm::vec3 spherePosition(0.0f, 1.0f, -10.0f);
GLfloat sphereScale = 1.5f;

glm::vec3 cubePosition(0.0f, 1.0f, -5.0f);
GLfloat cubeScale = 1.5f;

Scene* currentScene = nullptr;

unordered_map<PlaneObject*, Scene*> planeCubeMap;

void setup_illum_shader(Shader&);
void setup_cel_shading(Shader&);

glm::mat4 ModifyProjectionMatrix(const glm::mat4& projection, const glm::vec4& clipPlane);

GLfloat planeOffset()
{
    return ((planeScale + planeBorder) / 2.0f) * glm::cos(glm::radians(0.0f));
}

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
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "RGP_work", nullptr, nullptr);
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

    //the "clear" color for the frame buffer
    glClearColor(0.26f, 0.46f, 0.98f, 1.0f);
	
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    // we create the Shader Program used for objects (which presents different subroutines we can switch)
    Shader illum_shader = Shader("illumination_model.vert", "illumination_model.frag");
    Shader cel_shading = Shader("illumination_model.vert", "cel_shading.frag");
    Shader color_shader = Shader("basic.vert", "fullcolor.frag");
    Shader depth_shader = Shader("basic.vert", "depth.frag");
    // we parse the Shader Program to search for the number and names of the subroutines.
    // the names are placed in the shaders vector
    SetupShader(illum_shader.Program);
    // we print on console the name of the first subroutine used
    PrintCurrentShader(current_subroutine);

    Texture uvTexture("../../textures/UV_Grid_Sm.png");

    // we load the model(s) (code of Model class is in include/utils/model.h)
    ModelObject bunnyObject("../../models/bunny_lp.obj", cel_shading, bunnyPosition, bunnyScale);
    bunnyObject.setTexture(&uvTexture);
    bunnyObject.setColor(diffuseColor);

    // we load the model(s) (code of Model class is in include/utils/model.h)
    ModelObject sphereObject("../../models/sphere.obj", illum_shader, spherePosition, sphereScale);
    sphereObject.setColor(diffuseColor);

    ModelObject cubeObject("../../models/cube.obj", illum_shader, cubePosition, cubeScale);
    cubeObject.setColor(diffuseColor);

    ModelObject floorObject("../../models/plane.obj", illum_shader, glm::vec3(0.0f, -1.0f, 0.0f));
    floorObject.setScale(glm::vec3(10.0f, 1.0f, 10.0f));
    floorObject.setColor(groundColor);

    Scene mainScene;
    mainScene.add_object(&floorObject);
    mainScene.add_object(&sphereObject);

    currentScene = &mainScene;
    
    Scene frontScene;
    frontScene.add_object(&cubeObject);
    
    Scene rightScene;
    rightScene.add_object(&bunnyObject);
    
    ModelObject cubeObject2("../../models/cube.obj", illum_shader, cubePosition, cubeScale / 2);
    cubeObject2.setColor(glm::vec3(0.0f, 0.0f, 1.0f));
        
    ModelObject bunnyObject2("../../models/bunny_lp.obj", cel_shading, bunnyPosition, bunnyScale / 2);
    bunnyObject2.setColor(glm::vec3(0.0f, 0.0f, 1.0f));
    
    ModelObject cubeStructure("../../models/cube_structure.obj", illum_shader, cubeStructurePosition, cubeStructureScale);
    cubeStructure.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    
    Scene leftScene;
    leftScene.add_object(&cubeObject2);
    
    Scene backScene;
    backScene.add_object(&bunnyObject2);

    PlaneObject frontPlaneObject(color_shader, glm::vec3(0.0f, 0.0f, planeOffset()), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    frontPlaneObject.setColor(leftFrontBorderColor);
    
    PlaneObject rightPlaneObject(color_shader, glm::vec3(planeOffset(), 0.0f, 0.0f), 1.0f, glm::vec3(0.0f, 90.0f, 0.0f));
    rightPlaneObject.setColor(rightFrontBorderColor);
    
    PlaneObject leftPlaneObject(color_shader, glm::vec3(- planeOffset(), 0.0f, 0.0f), 1.0f, glm::vec3(0.0f, - 90.0f, 0.0f));
    leftPlaneObject.setColor(leftBackBorderColor);
    
    PlaneObject backPlaneObject(color_shader, glm::vec3(0.0f, 0.0f,- planeOffset()), 1.0f, glm::vec3(0.0f, 180.0f, 0.0f));
    backPlaneObject.setColor(rightBackBorderColor);

    cubeStructure.addChild(&frontPlaneObject);
    cubeStructure.addChild(&rightPlaneObject);
    cubeStructure.addChild(&leftPlaneObject);
    cubeStructure.addChild(&backPlaneObject);

    planeCubeMap[&frontPlaneObject] = &frontScene;
    planeCubeMap[&rightPlaneObject] = &rightScene;
    planeCubeMap[&leftPlaneObject] = &leftScene;
    planeCubeMap[&backPlaneObject] = &backScene;

    glm::mat4 view = glm::mat4(1.0f);

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

        // ImGUI window
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
			
			ImGui::SeparatorText("Colors");
            if (ImGui::ColorEdit3("Ground color", (float*)&groundColor)) { floorObject.setColor(groundColor); }
            if(ImGui::ColorEdit3("Objects color", (float*)&diffuseColor)) {
                sphereObject.setColor(diffuseColor);
                cubeObject.setColor(diffuseColor);
                bunnyObject.setColor(diffuseColor);
            }
			
            if (ImGui::CollapsingHeader("Portals"))
            {
                if (ImGui::InputFloat("Plane scale", &planeScale))
                {
                    frontPlaneObject.setScale(planeScale);
                    rightPlaneObject.setScale(planeScale);
                    frontPlaneObject.setPosition(glm::vec3(0.0f, 0.0f, planeOffset()));
                    rightPlaneObject.setPosition(glm::vec3(planeOffset(), 0.0f, 0.0f));

                    leftPlaneObject.setScale(planeScale);
                    backPlaneObject.setScale(planeScale);
                    leftPlaneObject.setPosition(glm::vec3(- planeOffset(), 0.0f, 0.0f));
                    backPlaneObject.setPosition(glm::vec3(0.0f, 0.0f, - planeOffset()));
                }
                if (ImGui::InputFloat("Plane border", &planeBorder))
                {
                    frontPlaneObject.setPosition(glm::vec3(0.0f, 0.0f, planeOffset()));
                    rightPlaneObject.setPosition(glm::vec3(planeOffset(), 0.0f, 0.0f));

                    leftPlaneObject.setPosition(glm::vec3(- planeOffset(), 0.0f, 0.0f));
                    backPlaneObject.setPosition(glm::vec3(0.0f, 0.0f, - planeOffset()));
                }
            
                ImGui::SeparatorText("PortalCube");
                if (ImGui::InputFloat3("CubeStructure position", (float*)&cubeStructurePosition)) { cubeStructure.setPosition(cubeStructurePosition); }
                if (ImGui::InputFloat3("CubeStructure rotation", (float*)&cubeStructureRotation)) { cubeStructure.setRotation(cubeStructureRotation); }
                if (ImGui::InputFloat("CubeStructure scale", &cubeStructureScale)) { cubeStructure.setScale(cubeStructureScale); }
            }
            
			ImGui::SeparatorText("Bunny");
			if (ImGui::InputFloat3("Bunny position", (float*)&bunnyPosition)) { bunnyObject.setPosition(bunnyPosition); }
            if (ImGui::InputFloat("Bunny scale", &bunnyScale)) { bunnyObject.setScale(bunnyScale); }
            
			ImGui::SeparatorText("Sphere");
			if (ImGui::InputFloat3("Sphere position", (float*)&spherePosition)) { sphereObject.setPosition(spherePosition); }
            if (ImGui::InputFloat("Sphere scale", &sphereScale)) { sphereObject.setScale(sphereScale); }
            
			ImGui::SeparatorText("Cube");
			if (ImGui::InputFloat3("Cube position", (float*)&cubePosition)) { cubeObject.setPosition(cubePosition); }
            if (ImGui::InputFloat("Cube scale", &cubeScale)) { cubeObject.setScale(cubeScale); }

			if(ImGui::CollapsingHeader("Illumination Model Configuration"))
			{
				ImGui::InputFloat3("Light Position", (float*)&lightPos0);
			    ImGui::SeparatorText("Cel shading parameters");
				ImGui::SliderFloat("Thickness", (float*)&celShadingThickness, 0.0f, 1.0f);
                ImGui::SeparatorText("Illumination model parameters");
				ImGui::Text("Illumination Model = %s", subroutines[current_subroutine]);
				ImGui::SliderFloat("Kd", (float*)&Kd, 0.0f, 2.0f);
				ImGui::SliderFloat("Ks", (float*)&Ks, 0.0f, 2.0f);
				ImGui::SliderFloat("Ka", (float*)&Ka, 0.0f, 5.0f);
				ImGui::SliderFloat("Shininess", (float*)&shininess, 0.0f, 100.0f);
				ImGui::SliderFloat("Alpha", (float*)&alpha, 0.0f, 5.0f);
				ImGui::SliderFloat("F0", (float*)&F0, 0.0f, 100.0f);
				ImGui::ColorEdit3("Specular color", (float*)&specularColor);
				ImGui::ColorEdit3("Ambient color", (float*)&ambientColor);
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
        
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        
        if (spinning)
        {
            orientationY+=(deltaTime*spin_speed);
            
            glm::vec3 portalCubeRotation = cubeStructure.rotation();
            portalCubeRotation.y = orientationY;
            cubeStructure.setRotation(portalCubeRotation);
        }
		
		/////////////////// STENCIL ///////////////////////////////////////////
        
        vector<PlaneObject*> visiblePlanes;
        
        glEnable(GL_STENCIL_TEST);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color drawing
        
        glStencilMask(0xFF); // Enable stencil drawing
        glDepthMask(0x00);
        
        glClear(GL_STENCIL_BUFFER_BIT);
        
        for (auto i = planeCubeMap.begin(); i != planeCubeMap.end(); ++i)
        {
            PlaneObject* plane = i->first;
            glm::vec3 normal = plane->worldNormal();
            glm::vec3 position = plane->worldPosition();
            glm::vec3 toCamera = glm::normalize(camera.Position - position);
            
            if(glm::dot(normal, toCamera) > 0) {
                visiblePlanes.push_back(plane);
            }
        }
        
        uint8_t visiblePlanesSize = visiblePlanes.size();
        
        for (uint8_t i = 0; i < visiblePlanesSize; ++i)
        {
            PlaneObject* plane = visiblePlanes[i];
            
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, i + 1, 0xFF);
            plane->setScale(glm::vec3(planeScale));
            
            plane->draw(view, projection);
        }
        
        /////////////////// OBJECTS IN PORTAL /////////////////////////////////
        
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Enable color drawing
        glStencilMask(0x00); // Disable stencil drawing
        glDepthMask(0xFF);
		
        setup_illum_shader(illum_shader);
        setup_cel_shading(cel_shading);
        
        for (uint8_t i = 0; i < visiblePlanesSize; ++i)
        {
            PlaneObject* plane = visiblePlanes[i];
            Scene* scene = planeCubeMap.at(plane);            
            
            glm::mat4 portalProjection = projection;
            
            glStencilFunc(GL_EQUAL, i + 1, 0xFF);
            
            glm::vec4 portalNormal = glm::normalize(glm::inverseTranspose(view * plane->modelMatrix()) * glm::vec4(0.0f, 0.0f, - 1.0f, 0.0f));
            portalProjection = ModifyProjectionMatrix(
                projection,
                portalNormal
            );
            
            scene->draw(view, portalProjection);

            if(planeBorder > 0) {
                glStencilFunc(GL_NOTEQUAL, i + 1, 0xFF);
                plane->setScale(glm::vec3(planeScale + planeBorder));
                plane->draw(view, projection);
            }
        }

        glDisable(GL_STENCIL_TEST); // Disable stencil test
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color drawing
        
        glClear(GL_DEPTH_BUFFER_BIT);

        for (auto i = visiblePlanes.begin(); i != visiblePlanes.end(); i++)
        {
            (*i)->draw(view, projection);
        }

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Enable color drawing
        
        /////////////////// OBJECTS OUTSIDE THE PORTAL ////////////////////////
        
        currentScene->draw(view, projection);
        cubeStructure.draw(view, projection);

        // Rendering imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    // when I exit from the graphics loop, it is because the application is closing
    // we delete the Shader Programs
    color_shader.Delete();
    illum_shader.Delete();
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

    if(key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        PlaneObject* observedPlane = nullptr;
        float observedDot = .75f;

        for (auto i = planeCubeMap.begin(); i != planeCubeMap.end(); i++)
        {
            PlaneObject* plane = i->first;
            glm::vec3 distanceFromCamera = plane->worldPosition() - camera.Position;
            distanceFromCamera.y = 0;
            float distance = glm::length(distanceFromCamera);
            if(distance >= 2.0f)
            {
                continue;
            }

            glm::vec3 portalNormal = glm::normalize(glm::inverseTranspose(glm::mat3(camera.GetViewMatrix() * plane->modelMatrix())) * glm::vec3(0.0f, 0.0f, - 1.0f));
            float dot = glm::dot(portalNormal, glm::vec3(0.0f, 0.0f, - 1.0f));

            if(dot > observedDot)
            {
                observedPlane = plane;
            }
        }

        cout << endl;

        if(observedPlane)
        {
            Scene* scene = planeCubeMap[observedPlane];
            planeCubeMap[observedPlane] = currentScene;
            currentScene = scene;
        }
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
    illum_shader.Use(subroutines[current_subroutine]);

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
    glUniform3fv(matAmbientLocation, 1, glm::value_ptr(ambientColor));
    glUniform3fv(matSpecularLocation, 1, glm::value_ptr(specularColor));
    glUniform1f(kdLocation, Kd);
    glUniform1f(ksLocation, Ks);
    glUniform1f(kaLocation, Ka);
    glUniform1f(shininessLocation, shininess);
    glUniform1f(alphaLocation, alpha);
    glUniform1f(f0Location, F0);
}

void setup_cel_shading(Shader& cel_shading)
{
    cel_shading.Use(subroutines[current_subroutine]);

    GLint pointLightLocation = glGetUniformLocation(cel_shading.Program, "pointLightPosition");
    GLint matSpecularLocation = glGetUniformLocation(cel_shading.Program, "specularColor");
    GLint matAmbientLocation = glGetUniformLocation(cel_shading.Program, "ambientColor");
    GLint thicknessLocation = glGetUniformLocation(cel_shading.Program, "thickness");
    
    glUniform3fv(pointLightLocation, 1, glm::value_ptr(lightPos0));
    glUniform3fv(matAmbientLocation, 1, glm::value_ptr(ambientColor));
    glUniform3fv(matSpecularLocation, 1, glm::value_ptr(specularColor));
    glUniform1f(thicknessLocation, celShadingThickness);
}

glm::mat4 ModifyProjectionMatrix(const glm::mat4& projection, const glm::vec4& clipPlane)
{
    glm::vec4 q;
    glm::mat4 matrix = projection;
    // Calculate the clip-space corner point opposite the clipping plane
    // as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
    // transform it into camera space by multiplying it
    // by the inverse of the projection matrix
    
    q.x = (glm::sign(clipPlane.x) + matrix[2][0]) / matrix[0][0];
    q.y = (glm::sign(clipPlane.y) + matrix[2][1]) / matrix[1][1];
    q.z = -1.0F;
    q.w = (1.0F + matrix[2][2]) / matrix[3][2];
    
    // Calculate the scaled plane vector
    glm::vec4 c = clipPlane * (2.0F / glm::dot(clipPlane, q));
    
    // Replace the third row of the projection matrix
    matrix[0][2] = c.x;
    matrix[1][2] = c.y;
    matrix[2][2] = c.z + 1.0F;
    matrix[3][2] = c.w;

    return matrix;
}