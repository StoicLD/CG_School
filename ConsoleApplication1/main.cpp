#include <iostream>    
#include <ctime>
#include <cmath>
#include <algorithm>
#include <vector>
// GLEW    
#define GLEW_STATIC    
#include <glad.h>    
// GLFW    
#include <GLFW/glfw3.h>    
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::vector<float> Bresenham(int x0, int y0, int x1, int y1);
std::vector<float> DDA(int x0, int y0, int x1, int y1);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 460 core\n"
"layout (location = 1) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 初始设置环节结束


    // build and compile our shader program
    // ------------------------------------
    // (1) vertex shader
    // 创建一个顶点着色器，ID是vertexShader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 将对应的shader代码绑定到这个新创建的顶点着色器上面，vertexShaderSource就是对应的代码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // 编译shader
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // (2)fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // 到这里为止，shader编译链接完成

    // 下面准备顶点的数据
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // 当VAO还是存活的时候不要解绑EBO，因为VAO中是指向EBO的
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#pragma region Bresenham
    //(1) Bresenham算法
    // render
    // 背景颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 开始时间
    clock_t begin = clock();
    for (int j = 0; j <= 1000000; j++)
    {
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        int x0 = rand() % 10 - 10;
        int x1 = rand() % 10 + 10;
        int y0 = rand() % 10 - 10;
        int y1 = rand() % 10 + 10;
        std::vector<float> newVertices = Bresenham(x0, y0, x1, y1);
        //记得要归一化坐标
        for (int i = 0; i < newVertices.size(); i++)
        {
            if (i % 3 == 0)
                newVertices[i] /= SCR_WIDTH;
            if (i % 3 == 1)
                newVertices[i] /= SCR_HEIGHT;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //同一时间只有一个VBO绑定到GL_ARRAY_BUFFER这种类型的buffer上
        glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), &newVertices[0], GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //告诉OpenGL如何解释顶点数据
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        //激活index为1的属性，从vertex shader可以看出，我们 layout (location = 1)，也就是第一个属性
        glEnableVertexAttribArray(1);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawArrays(GL_POINTS, 0, newVertices.size() / 3);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 
        if (j == 1000)
        {
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "运行Bresenham算法1000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
        else if (j == 10000)
        {
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "运行Bresenham算法10000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
        else if (j == 100000)
        {
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "运行Bresenham算法100000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
        else if (j == 1000000)
        {
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "运行Bresenham算法1000000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
    }
    glfwSwapBuffers(window);
#pragma endregion


#pragma region DDA
    //(2) DDA算法
    // render
    // 背景颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 开始时间
    clock_t begin2 = clock();
    for (int j = 0; j <= 1000000; j++)
    {
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        int x0 = rand() % 10 - 10;
        int x1 = rand() % 10 + 10;
        int y0 = rand() % 10 - 10;
        int y1 = rand() % 10 + 10;
        std::vector<float> newVertices = DDA(x0, y0, x1, y1);
        //记得要归一化坐标
        for (int i = 0; i < newVertices.size(); i++)
        {
            if (i % 3 == 0)
                newVertices[i] /= SCR_WIDTH;
            if (i % 3 == 1)
                newVertices[i] /= SCR_HEIGHT;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //同一时间只有一个VBO绑定到GL_ARRAY_BUFFER这种类型的buffer上
        glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), &newVertices[0], GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //告诉OpenGL如何解释顶点数据
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        //激活index为1的属性，从vertex shader可以看出，我们 layout (location = 1)，也就是第一个属性
        glEnableVertexAttribArray(1);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawArrays(GL_POINTS, 0, newVertices.size() / 3);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 
        if (j == 1000)
        {
            clock_t end2 = clock();
            double elapsed_secs = double(end2 - begin2) / CLOCKS_PER_SEC;
            std::cout << "运行DDA算法1000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
        else if (j == 10000)
        {
            clock_t end2 = clock();
            double elapsed_secs = double(end2 - begin2) / CLOCKS_PER_SEC;
            std::cout << "运行DDA算法10000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
        else if (j == 100000)
        {
            clock_t end2 = clock();
            double elapsed_secs = double(end2 - begin2) / CLOCKS_PER_SEC;
            std::cout << "运行DDA算法100000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
        else if (j == 1000000)
        {
            clock_t end2 = clock();
            double elapsed_secs = double(end2 - begin2) / CLOCKS_PER_SEC;
            std::cout << "运行DDA算法1000000次，耗时 " << elapsed_secs << " 秒" << std::endl;
        }
    }
    glfwSwapBuffers(window);
#pragma endregion


    while (!glfwWindowShouldClose(window))
    {
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(shaderProgram);
        //glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //std::vector<float> newVertices = DDA(1, 1, 100, 200);
        ////记得要归一化坐标
        //for (int i = 0; i < newVertices.size(); i++)
        //{
        //    if(i % 3 == 0)
        //        newVertices[i] /= SCR_WIDTH;
        //    else if (i % 3 == 1)
        //        newVertices[i] /= SCR_HEIGHT;
        //}
        //glBindBuffer(GL_ARRAY_BUFFER, VBO);
        ////同一时间只有一个VBO绑定到GL_ARRAY_BUFFER这种类型的buffer上
        //glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), &newVertices[0], GL_STATIC_DRAW);
        ////glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        ////告诉OpenGL如何解释顶点数据
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        ////激活index为1的属性，从vertex shader可以看出，我们 layout (location = 1)，也就是第一个属性
        //glEnableVertexAttribArray(1);
        ////glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawArrays(GL_POINTS, 0, newVertices.size() / 3);
        ////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //// glBindVertexArray(0); // no need to unbind it every time
        //glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

std::vector<float> Bresenham(int x0, int y0, int x1, int y1)
{
    std::vector<float> drawingPoints;
    drawingPoints.reserve(30);
    if (x0 == x1)
    {
        drawingPoints.push_back(x0);
        drawingPoints.push_back(y0);
        drawingPoints.push_back(0);
    }
    else
    {
        float k = (float)(y1 - y0) / (float)(x1 - x0);
        if (abs(k) >= 1)
        {
            //保证y0小，for循环是递增的
            if (y0 > y1)
            {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }
            // 加入起始点
            drawingPoints.push_back(x0);
            drawingPoints.push_back(y0);
            drawingPoints.push_back(0);

            // 斜率绝对值大于等于1的情况, y每次递增1. x递增1/k
            int dx = x1 - x0;
            int dy = y1 - y0;
            float errorX = 2 * dy;

            for (int currY = y0, currX = x0; currY < y1; currY++)
            {
                // e = 2 * dx
                //float realX =  (float)currX + (float)(1.0 / k);
                int drawX = currX;
                int drawY = currY + 1;
                //误差累积
                errorX += 2 * dx;
                if (errorX > 0)
                {
                    // 画在
                    drawX += (k > 0 ? 1 : 0);
                    errorX -= 2 * dy;
                }
                //draw point
                drawingPoints.push_back(drawX);
                drawingPoints.push_back(drawY);
                drawingPoints.push_back(0);
                currX = drawX;
            }
        }
        else
        {
            //保证x0小，for循环是递增的
            if (x0 > x1)
            {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            drawingPoints.push_back(x0);
            drawingPoints.push_back(y0);
            drawingPoints.push_back(0);
            // 斜率绝对值大于等于1的情况, x每次递增1. y递增k
            int dx = x1 - x0;
            int dy = y1 - y0;
            int errorY = 2 * dx;
            // draw beginning point 
            for (int currX = x0, currY = y0; currX < x1; currX++)
            {
                //float realY = (float)currY + k;
                int drawY = currY;
                int drawX = currX + 1;
                //误差累积
                errorY += 2 * dy;
                if (errorY > 0)
                {
                    // 画在下一个点上
                    drawY += (k > 0 ? 1 : 0);
                    errorY -= 2 * dx;
                }
                //draw point
                drawingPoints.push_back(drawX);
                drawingPoints.push_back(drawY);
                drawingPoints.push_back(0);
                currY = drawY;
            }
        }
    }
    return drawingPoints;
}


std::vector<float> DDA(int x0, int y0, int x1, int y1)
{
    std::vector<float> drawingPoints;
    drawingPoints.reserve(30);
    if (x0 == x1)
    {
        drawingPoints.push_back(x0);
        drawingPoints.push_back(y0);
        drawingPoints.push_back(0);
    }
    else
    {
        float k = (float)(y1 - y0) / (float)(x1 - x0);
        if (abs(k) >= 1)
        {
            //保证y0小，for循环是递增的
            if (y0 > y1)
            {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }
            // 加入起始点
            drawingPoints.push_back(x0);
            drawingPoints.push_back(y0);
            drawingPoints.push_back(0);

            // 斜率绝对值大于等于1的情况, y每次递增1. x递增1/k
            int dx = x1 - x0;
            int dy = y1 - y0;
            int step = std::max(dx, dy);
            float xIncrement = (float)dx / (float)step;
            float yIncrement = (float)dy / (float)step;

            for (float currY = y0, currX = x0; currY < y1;)
            {
                currX = currX + xIncrement;
                currY = currY + yIncrement;
                //draw point
                drawingPoints.push_back(round(currX));
                drawingPoints.push_back(round(currY));
                drawingPoints.push_back(0);
            }
        }
        else
        {
            //保证x0小，for循环是递增的
            if (x0 > x1)
            {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            drawingPoints.push_back(x0);
            drawingPoints.push_back(y0);
            drawingPoints.push_back(0);
            // 斜率绝对值大于等于1的情况, x每次递增1. y递增k
            int dx = x1 - x0;
            int dy = y1 - y0;
            int step = std::max(dx, dy);
            float xIncrement = dx / step;
            float yIncrement = dy / step;
            for (int currX = x0, currY = y0; currX < x1;)
            {
                currX = currX + xIncrement;
                currY = currY + yIncrement;
                //draw point
                drawingPoints.push_back(round(currX));
                drawingPoints.push_back(round(currY));
                drawingPoints.push_back(0);
            }
        }
    }
    return drawingPoints;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}