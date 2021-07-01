#include <glad/glad.h>
#include "../glpp/glpp.hpp"
#include <iostream>
#include <string>

const char* containerPath = "D:\\w\\learn\\opengl\\proj1\\test1\\test1\\container.jpg";
const char* awesomefacePath = "D:\\w\\learn\\opengl\\proj1\\test1\\test1\\awesomeface.png";
//const char* awesomefacePath = "D:\\w\\learn\\opengl\\proj1\\test1\\test1\\DSC00603.jpg";
const glm::mat4 k_identity(1.0f), k_zero ( 0.0f);

glm::vec3 cubePositions[] = {
glm::vec3(0.0f,  0.0f,  0.0f),
glm::vec3(2.0f,  5.0f, -15.0f),
glm::vec3(-1.5f, -2.2f, -2.5f),
glm::vec3(-3.8f, -2.0f, -12.3f),
glm::vec3(2.4f, -0.4f, -3.5f),
glm::vec3(-1.7f,  3.0f, -7.5f),
glm::vec3(1.3f, -2.0f, -2.5f),
glm::vec3(1.5f,  2.0f, -2.5f),
glm::vec3(1.5f,  0.2f, -1.5f),
glm::vec3(-1.3f,  1.0f, -1.5f)
};


unsigned int VAO, EBO;
//unsigned int texture;
unsigned int texture1, texture2;

unsigned int loadTexture(glpp::ntstr_view path) {
    auto texContainerImage = glpp::loadImage(path);
    auto format = texContainerImage.channels() == 3 ? GL_RGB : GL_RGBA;
    unsigned int tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texContainerImage.width(), texContainerImage.height(), 0, format, GL_UNSIGNED_BYTE, texContainerImage.raw_data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glBindTexture(GL_TEXTURE_2D, 0);

    return tex_id;
}

void foo(){
//////// Begin Data

    float vertices[] = {
    // back
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // front
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, 0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, 0.5f,  0.0f, 1.0f,
    // left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // right
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // top
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // bottom
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    };
//
//    float vertices[] = {
//-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
// 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
// -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
// 0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
//    };
    unsigned int indices[] = { 
    0, 1, 2, 
    2, 3, 0,
    
    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15,12,
    
    16,17,18,
    18,19,16,

    20, 21, 22,
    22, 23, 20,
    };

//////// Begin Vertex
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

//////// Begin texture
    texture1 = loadTexture(containerPath);
    texture2 = loadTexture(awesomefacePath);

}


const std::string_view vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offsets[10];

uniform float ts;

out vec2 TexCoord;


mat4 getModel(mat4 m, float angle, vec3 v){
		float a = angle;
		float c = cos(a);
		float s = sin(a);

		vec3 axis = normalize(v);
		vec3 temp = (1.0 - c) * axis;

		mat4 Rotate = mat4(1.0);
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		mat4 Result = mat4(1.0);
		Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
		Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
		Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
		Result[3] = m[3];
		return Result;
}

void main()
{
    vec3 offset = offsets[gl_InstanceID];
    mat4 m = mat4(1.0);
    m[3]  = m[0] * offset[0] + m[1] * offset[1] + m[2] * offset[2] + m[3]; //traslate
    m =  getModel(m, ts * (gl_InstanceID + 1) * 0.4,vec3(1.0f, 0.3f, 0.5f)); //rotate

    gl_Position = projection * view * m * vec4(aPos, 1.0) ;//+ vec4(offset, 1);

    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}   
)";
const std::string_view fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
} 
)";

glpp::Program createProgram() {
    auto vshader = glpp::Shader::makeVertex(vertexShaderSource);
    auto fshader = glpp::Shader::makeFragment(fragmentShaderSource);

    return glpp::ProgramBuilder()
        .addShader(vshader)
        .addShader(fshader)
        .build();
}
// settings
const glpp::rect_scale_t k_src_rect{ 800 , 800 };

//= mat3(0.5, -0.816, 0.25,
//    0.816, 0.5, 0.25,
//    0, 0, 1);
int main()
{
    auto context = glpp::GlobalContext::init();

    // glfw window creation
    // --------------------
    auto window = glpp::Window::make(k_src_rect, "LearnOpenGL");
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGL())
    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // render loop
    // -----------
    foo();
    auto program =  createProgram();



    //int vertexColorLocation = program.index_of_uniform("ourColor");
 
    //glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    program.use();

    glUniform1i(program.index_of_uniform("texture1"), 0);
    glUniform1i(program.index_of_uniform("texture2"), 1);
    
    //glm::mat4 flip(1.0f);
    //flip[1].y = -1.0f;
    //glm::mat4 model = glm::rotate(k_identity, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * flip;
    //glm::mat4 view = glm::translate(k_identity, glm::vec3(0.0f, 0.0f, -3.0f));
    //glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(k_src_rect.width) / float(k_src_rect.height), 0.1f, 100.0f);

    glm::mat4 model = glm::rotate(k_identity, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::translate(k_identity, glm::vec3(2.0f, 2.0f, -5.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(k_src_rect.width) / float(k_src_rect.height), 0.1f, 100.0f);

    glUniformMatrix4fv(program.index_of_uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(program.index_of_uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(program.index_of_uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(program.index_of_uniform("offsets"), 30, &cubePositions[0].x);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glEnable(GL_DEPTH_TEST);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //bool abranch = true;
    while (!window.getShouldClose())
    {
        // input
        // -----
        if (window.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
            window.setShouldClose(1);

        // render
        // ------
        glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glm::mat4 trans(1.0f);
        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 2.0f, 3.0f));
        //glUniformMatrix4fv(program.index_of_uniform("transform"), 1, GL_FALSE, glm::value_ptr(trans));
        //model = glm::rotate(k_identity, (float)glfwGetTime() * glm::radians(60.f), glm::vec3(1.0f, 0.3f, 0.5f));
        //model = glm::rotate(k_identity, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //glUniformMatrix4fv(program.index_of_uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(program.index_of_uniform("ts"), (float)glfwGetTime());
 //       float timeValue = float(glfwGetTime());
        //float greenValue = abs(sin(timeValue*3.925) *0.8) + 0.2f;
 //       float greenValue = 1.0f;
 //       glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 10);

        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    model = glm::translate(k_identity, cubePositions[i]);
        //    float angle = 20.0f * (i + 1);
        //    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //    //ourShader.setMat4("model", model);
        //    glUniformMatrix4fv(program.index_of_uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
        //    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //    //glDrawArrays(GL_TRIANGLES, 0, 36);
        //}

        //        glDrawArrays(GL_TRIANGLES, 0, 3);
//        if ((int(timeValue) & 1) == 0)
            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //else
        //    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, (void*)8);

        //abranch = !abranch;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.swapBuffer();
        //glfwWaitEvents();
        glfwWaitEventsTimeout(0.01);
        //glfwPollEvents();
    }

    return 0;
}
