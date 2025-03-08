#include <string>

#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>

#include <glfw/glfw3.h>

#ifdef _WINDOWS_
    #error windows.h was included!
#endif

#include <utils/shader.h>
#include <utils/texture.h>
#include <utils/model.h>
#include <utils/camera.h>

#include <utils/modelobject.h>
#include <utils/planeobject.h>
#include <utils/scene.h>
#include <utils/shaderscene.h>
#include "sketchshaderscene.h"
#include "celshadingshaderscene.h"
#include "paintingshaderscene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>

static const string SHADER_PATH = "../../shaders/";

GLuint screenWidth = 1920, screenHeight = 1080;
GLfloat fieldOfViewY = 45.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void apply_camera_movements();
bool canMoveCamera = false;

GLuint current_subroutine = 0;
vector<std::string> subroutines;
void SetupShader(int shader_program);
void PrintCurrentShader(int subroutine);

bool keys[1024];
GLfloat lastX, lastY;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat orientationY = 0.0f;
GLfloat positionY = 0.0f;
GLfloat spin_speed = 10.0f;
GLfloat float_speed = 2.0f;
GLfloat float_amount = 0.08f;

GLboolean spinning = GL_FALSE;
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

glm::vec3 groundColor(0.0f, 0.5f, 0.0f);

GLfloat planeScale = 0.95f;
GLfloat planeBorder = 0.0f;

glm::vec3 bunnyPosition(0.0f, 1.0f, -5.0f);
GLfloat bunnyScale = 0.5f;

glm::vec3 cubeStructurePosition(0.0f, 0.5f, 0.0f);
glm::vec3 cubeStructureRotation(0.0f, 0.0f, 0.0f);
GLfloat cubeStructureScale = 2.5f;

glm::vec3 mainScenePosition(0.0f, 0.0f, 0.0f);
glm::vec3 mainSceneRotation(0.0f, 0.0f, 0.0f);
GLfloat mainSceneScale = 1.0f;

glm::vec3 spherePosition(0.0f, 1.0f, -10.0f);
GLfloat sphereScale = 1.5f;

glm::vec3 cubePosition(0.0f, 1.0f, -5.0f);
GLfloat cubeScale = 1.5f;

ShaderScene* currentScene = nullptr;

unordered_map<PlaneObject*, ShaderScene*> planeCubeMap;

void setup_illum_shader(Shader&);

glm::mat4 ModifyProjectionMatrix(const glm::mat4& projection, const glm::vec4& clipPlane);

GLfloat planeOffset()
{
    return ((planeScale + planeBorder) / 2.0f) * glm::cos(glm::radians(0.0f));
}

/////////////////// MAIN function ///////////////////////
int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

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

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

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
    
    Shader illum_shader = Shader(
        (SHADER_PATH + "illumination_model.vert").c_str(),
        (SHADER_PATH + "illumination_model.frag").c_str()
    );
    Shader color_shader = Shader(
        (SHADER_PATH + "basic.vert").c_str(),
        (SHADER_PATH + "fullcolor.frag").c_str()
    );
    Shader depth_shader = Shader(
        (SHADER_PATH + "basic.vert").c_str(),
        (SHADER_PATH + "depth.frag").c_str()
    );

    SetupShader(illum_shader.Program);
    PrintCurrentShader(current_subroutine);


    ModelObject sphereObject("Sphere", "../../models/sphere.obj", illum_shader, spherePosition, sphereScale);
    sphereObject.setColor(diffuseColor);

    ModelObject floorObject("Floor", "../../models/plane.obj", illum_shader, glm::vec3(0.0f, -1.0f, 0.0f));
    floorObject.setScale(glm::vec3(10.0f, 1.0f, 10.0f));
    floorObject.setColor(groundColor);

    ShaderScene mainScene("MainScene");
    mainScene.add_external_object(&floorObject);
    mainScene.add_external_object(&sphereObject);

    currentScene = &mainScene;
    
    SketchShaderScene sketchScene("Sketch Scene", window, width, height);
    sketchScene.setup_scene();
    
    CelShadingShaderScene celShadingScene("CelShading Scene", window, width, height);
    celShadingScene.setup_scene();
    
    ModelObject bunnyObject2("Bunny_2", "../../models/bunny_lp.obj", illum_shader, bunnyPosition, bunnyScale / 2);
    bunnyObject2.setColor(glm::vec3(0.0f, 0.0f, 1.0f));
    
    ModelObject cubeStructure("Cube structure", "../../models/cube_structure.obj", illum_shader, cubeStructurePosition, cubeStructureScale);
    cubeStructure.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    
    PaintingShaderScene paintingScene("Painting Scene", window, width, height);
    paintingScene.setup_scene();
    
    ShaderScene backScene("BackScene");
    backScene.add_external_object(&bunnyObject2);

    PlaneObject frontPlaneObject("Plane front", color_shader, glm::vec3(0.0f, 0.0f, planeOffset()), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    frontPlaneObject.setColor(leftFrontBorderColor);
    
    PlaneObject rightPlaneObject("Plane right", color_shader, glm::vec3(planeOffset(), 0.0f, 0.0f), 1.0f, glm::vec3(0.0f, 90.0f, 0.0f));
    rightPlaneObject.setColor(rightFrontBorderColor);
    
    PlaneObject leftPlaneObject("Plane left", color_shader, glm::vec3(- planeOffset(), 0.0f, 0.0f), 1.0f, glm::vec3(0.0f, - 90.0f, 0.0f));
    leftPlaneObject.setColor(leftBackBorderColor);
    
    PlaneObject backPlaneObject("Plane back", color_shader, glm::vec3(0.0f, 0.0f,- planeOffset()), 1.0f, glm::vec3(0.0f, 180.0f, 0.0f));
    backPlaneObject.setColor(rightBackBorderColor);

    cubeStructure.addChild(&frontPlaneObject);
    cubeStructure.addChild(&rightPlaneObject);
    cubeStructure.addChild(&leftPlaneObject);
    cubeStructure.addChild(&backPlaneObject);

    planeCubeMap[&frontPlaneObject] = &sketchScene;
    planeCubeMap[&rightPlaneObject] = &celShadingScene;
    planeCubeMap[&leftPlaneObject] = &paintingScene;
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
			
            ImGui::SeparatorText("Camera");
			if(ImGui::SliderFloat("FoV X", &fieldOfViewX, 0.0f, 180.0f)) {
				fieldOfViewY = glm::degrees(glm::atan(glm::tan(glm::radians(fieldOfViewX) / 2) * screenHeight / screenWidth) * 2);
			}
			ImGui::SliderFloat("FoV Y", &fieldOfViewY, 0.0f, 180.0f);
            ImGui::Checkbox("On ground", &camera.onGround);
            ImGui::InputFloat3("Position", (float*)&camera.Position);
            ImGui::SliderFloat("Speed", &camera.MovementSpeed, 0.0f, 20.0f);

            ImGui::SeparatorText("Properties");
            ImGui::Checkbox("Wireframe", (bool*)&wireframe);
            ImGui::Checkbox("Spinning", (bool*)&spinning);
            if (ImGui::TreeNode("Spinning options")) {
                ImGui::SliderFloat("Rotation speed", &spin_speed, 0.0f, 180.0f);
                ImGui::SliderFloat("Floating speed", &float_speed, 0.0f, 50.0f);
                ImGui::SliderFloat("Floating amount", &float_amount, 0.0f, 1.0f);

                ImGui::TreePop();
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
                if (ImGui::DragFloat3("CubeStructure position", (float*)&cubeStructurePosition)) { cubeStructure.setPosition(cubeStructurePosition); }
                if (ImGui::DragFloat3("CubeStructure rotation", (float*)&cubeStructureRotation)) { cubeStructure.setRotation(cubeStructureRotation); }
                if (ImGui::InputFloat("CubeStructure scale", &cubeStructureScale)) { cubeStructure.setScale(cubeStructureScale); }
            }

            ImGui::NewLine();

            ImGui::SeparatorText("Current scene");
            currentScene->drawImGui();
            ImGui::SeparatorText("Cube scenes");
            for (auto i = planeCubeMap.cbegin(); i != planeCubeMap.cend(); ++i)
            {
                (*i->second).drawImGui();
            }
            
            ImGui::Separator();
            ImGui::NewLine();

			if(ImGui::CollapsingHeader("Illumination Model Configuration"))
			{
				ImGui::DragFloat3("Light Position", (float*)&lightPos0);
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
        
        if (spinning)
        {
            orientationY += deltaTime * spin_speed;
            orientationY = glm::mod(orientationY, 360.0f);
            positionY += deltaTime * float_speed;
            positionY = glm::mod(positionY, 2 * glm::pi<float>());
            
            glm::vec3 portalCubeRotation = cubeStructure.rotation();
            glm::vec3 portalCubePosition = cubeStructure.position();
            portalCubeRotation.y = orientationY;
            portalCubePosition.y = cubeStructurePosition.y + glm::sin(positionY) * float_amount;
            cubeStructure.setRotation(portalCubeRotation);
            cubeStructure.setPosition(portalCubePosition);
        }
        
        /////////////////// SETUP /////////////////////////////////////////////
        
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Disable color drawing
        
        glClearColor(0.26f, 0.46f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /////////////////// OBJECTS OUTSIDE THE PORTAL ////////////////////////
        
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        
        currentScene->update_scene(&camera, view, projection);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        currentScene->draw(view, projection);

        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        
        cubeStructure.draw(view, projection);
        
		/////////////////// STENCIL ///////////////////////////////////////////
        
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color drawing
        
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF); // Enable stencil drawing
        glClear(GL_STENCIL_BUFFER_BIT);

        vector<PlaneObject*> visiblePlanes;
        
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
        
        for (uint8_t i = 0; i < visiblePlanesSize; ++i)
        {
            PlaneObject* plane = visiblePlanes[i];
            ShaderScene* scene = planeCubeMap.at(plane);            
            
            glm::mat4 portalProjection = projection;
            
            glStencilFunc(GL_EQUAL, i + 1, 0xFF);
            
            glm::vec4 portalNormal = glm::normalize(glm::inverseTranspose(view * plane->modelMatrix()) * glm::vec4(0.0f, 0.0f, - 1.0f, 0.0f));
            portalProjection = ModifyProjectionMatrix(
                projection,
                portalNormal
            );
            
            glClear(GL_DEPTH_BUFFER_BIT);
            scene->update_scene(&camera, view, portalProjection);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            scene->draw(view, projection);
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            
            if(planeBorder > 0) {
                glStencilFunc(GL_NOTEQUAL, i + 1, 0xFF);
                plane->setScale(glm::vec3(planeScale + planeBorder));
                plane->draw(view, projection);
            }
        }

        ///////////////////////////////////////////////////////////////////////
        
        // Rendering imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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
            ShaderScene* scene = planeCubeMap[observedPlane];
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