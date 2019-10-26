// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
using namespace std;
//#define MyRand(x) static_cast <float> (rand() * x) / static_cast <float> (RAND_MAX);

struct Point
{
    float x;
    float y;
    float z;
public:
    Point()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;

    }
    Point(float _x, float _y, float _z)
    {
        this->x = _x;
        this->y = _y;
        this->z = _z;
    }
};


void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow *window);
vector<Point> ConHen(Point p0, Point p1, Point lineP0, Point lineP1, Point lineP2, Point lineP3);
float RandFloat();
void EnCode(int &code, Point p0, Point lineP0, Point lineP1, Point lineP2, Point lineP3);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//写死的shader string
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
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
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#pragma region Build Shader
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
    // fragment shader
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

#pragma endregion

#pragma region SetUp vertex data and buffers
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1,   // first Triangle
        1, 2,   // second Triangle
        2, 3,
        3, 0
    };
    unsigned int VBO, VAO, EBO;
    //生成buffer序号
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // 先绑定VAO
    glBindVertexArray(VAO);
    // 然后绑定VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 这一步把数据复制到buffer里面
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 同一时间 同一类型只能绑定一个buffer，下一个会替换上一个
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 告诉OpenGL怎么解释vertex buffer中的数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // 和shader中的属性位置对应，vertex shader中设置的是location 0为坐标这一属性，因此要Enable这一属性。
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    // 这个感觉是内存中的数据在了，glVertexAttribPointer这一步以及注册了对应的buffer，因此可以解除绑定了。
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // 因为我要有多个VAO，所以我先解绑
    glBindVertexArray(0);


#pragma endregion



#pragma region Get ConHen Points
    Point lineP0(-0.5f, 0.5f, 0);
    Point lineP1(0.5f, 0.5f, 0);
    Point lineP2(0.5f, -0.5f, 0);
    Point lineP3(-0.5f, -0.5f, 0);
    vector<Point> testPoints;
    for (int i = 0; i < 100; i++)
    {
        Point p0(RandFloat(), RandFloat(), 0);
        Point p1(RandFloat(), RandFloat(), 0);
        //Point p0(0.7f, 0.6f, 0);
        //Point p1(0.1f, 0, 0);

        vector<Point> points = ConHen(p0, p1, lineP0, lineP1, lineP2, lineP3);
        testPoints.insert(testPoints.end(), points.begin(), points.end());
        //testPoints.push_back(p0);
        //testPoints.push_back(p1);

    }
    //Point p0(0.6f, 0.9f, 0);
    //Point p1(0.1f, 0, 0);

    //vector<Point> points = ConHen(p0, p1, lineP0, lineP1, lineP2, lineP3);
    //testPoints.insert(testPoints.end(), points.begin(), points.end());




    unsigned int VAO_Points, VBO_Points;
    glGenVertexArrays(1, &VAO_Points);
    glBindVertexArray(VAO_Points);


    glGenBuffers(1, &VBO_Points);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Points);
    glBufferData(GL_ARRAY_BUFFER, testPoints.size() * sizeof(Point), &testPoints[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //这个函数和当前的VAO相关，如果我把这个函数注释掉了，那么不会绘制出图像
    //每次的enable，针对的是当前的VAO，VAO保存有VertexAttribute相关的信息和指向VBO的指针。
    glEnableVertexAttribArray(0);

#pragma endregion

    // render
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render loop
    // -----------
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        ProcessInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);


        //glBindBuffer(GL_ARRAY_BUFFER, VBO_Points);
        glBindVertexArray(VAO_Points); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        for (int i = 0; i < testPoints.size(); i += 2)
        {
            glDrawArrays(GL_LINE_STRIP, i, 2);
        }

        //glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        glDrawElements(GL_LINE_STRIP, 8, GL_UNSIGNED_INT, 0);

        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
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

void EnCode(int &areaCode, Point p0, Point lineP0, Point lineP1, Point lineP2, Point lineP3)
{
    if (p0.y > lineP0.y)
    {
        areaCode |= 0b1000;
    }
    else
    {
        areaCode &= 0b0111;
    }
    if (p0.x > lineP1.x)
    {
        areaCode |= 0b0100;
    }
    else
    {
        areaCode &= 0b1011;
    }
    if (p0.y < lineP2.y)
    {
        areaCode |= 0b0010;
    }
    else
    {
        areaCode &= 0b1101;
    }
    if (p0.x < lineP0.x)
    {
        areaCode |= 0b0001;
    }
    else
    {
        areaCode &= 0b1110;
    }
}


// 对于传入的两个点使用ConHen算法，返回两个在屏幕内的点
// 约定lineP0 --》 P3 分别为左上，右上，右下，坐下四个点
vector<Point> ConHen(Point p0, Point p1, Point lineP0, Point lineP1, Point lineP2, Point lineP3)
{
    //（1）计算两个点的对应4位二进制码，
    // 0000 上右下左 内部
    int areaP0 = 0b0000;
    int areaP1 = 0b0000;

    Point finalP0 = p0;
    Point finalP1 = p1;

    bool done = true;
    while (done)
    {
        EnCode(areaP0, finalP0, lineP0, lineP1, lineP2, lineP3);
        EnCode(areaP1, finalP1, lineP0, lineP1, lineP2, lineP3);
        //两个点都在内部
        if ((areaP0 | areaP1) == 0b0000)
            break;
        //两个点都完全在一条边界线的外部，表示该线段完全在范围之外
        //返回一个空的vector
        if ((areaP0 & areaP1) != 0b0000)
        {
            return vector<Point>();
        }

        //开始剪裁
        if (areaP0 != 0b0000)
        {
            if ((areaP0 | 0b1000) == areaP0)
            {
                //在上方线框外部，改变坐标到交点位置
                finalP0.y = lineP0.y;
                if (p1.y - p0.y != 0)
                {
                    finalP0.x = (p1.x - p0.x) * (finalP0.y - p0.y) / (p1.y - p0.y) + p0.x;
                }
                continue;
            }
            if ((areaP0 | 0b0100) == areaP0)
            {
                //在右方线框外部
                finalP0.x = lineP1.x;
                if (p1.x - p0.x != 0)
                {
                    finalP0.y = (p1.y - p0.y) * (finalP0.x - p0.x) / (p1.x - p0.x) + p0.y;
                }
                continue;
            }
            if ((areaP0 | 0b0010) == areaP0)
            {
                //在下方线框外部
                finalP0.y = lineP2.y;
                if (p1.y - p0.y != 0)
                {
                    finalP0.x = (p1.x - p0.x) * (finalP0.y - p0.y) / (p1.y - p0.y) + p0.x;
                }
                continue;
            }
            if ((areaP0 | 0b0001) == areaP0)
            {
                //在左方线框外部
                finalP0.x = lineP0.x;
                if (p1.x - p0.x != 0)
                {
                    finalP0.y = (p1.y - p0.y) * (finalP0.x - p0.x) / (p1.x - p0.x) + p0.y;
                }
                areaP0 ^= 0b0010;
                continue;
            }
        }
        if (areaP1 != 0b0000)
        {
            if ((areaP1 | 0b1000) == areaP1)
            {
                //在上方线框外部
                finalP1.y = lineP0.y;
                if (p1.y - p0.y != 0)
                {
                    finalP1.x = (p1.x - p0.x) * (finalP1.y - p0.y) / (p1.y - p0.y) + p0.x;
                }
                continue;
            }
            if ((areaP1 | 0b0100) == areaP1)
            {
                //在右方线框外部
                finalP1.x = lineP1.x;
                if (p1.x - p0.x != 0)
                {
                    finalP1.y = (p1.y - p0.y) * (finalP1.x - p0.x) / (p1.x - p0.x) + p0.y;
                }
                continue;
            }
            if ((areaP1 | 0b0010) == areaP1)
            {
                //在下方线框外部
                finalP1.y = lineP2.y;
                if (p1.y - p0.y != 0)
                {
                    finalP1.x = (p1.x - p0.x) * (finalP1.y - p0.y) / (p1.y - p0.y) + p0.x;
                }
                continue;
            }
            if ((areaP1 | 0b0001) == areaP1)
            {
                //在左方线框外部
                finalP1.x = lineP0.x;
                if (p1.x - p0.x != 0)
                {
                    finalP1.y = (p1.y - p0.y) * (finalP1.x - p0.x) / (p1.x - p0.x) + p0.y;
                }
                continue;
            }
        }
        break;
    }
    //if (areaP0 != 0b0000)
    //{
    //    if ((areaP0 | 0b1000) == areaP0)
    //    {
    //        //在上方线框外部，改变坐标到交点位置
    //        finalP0.y = lineP0.y;
    //        if (p1.y - p0.y != 0)
    //        {
    //            finalP0.x = (p1.x - p0.x) * (finalP0.y - p0.y) / (p1.y - p0.y) + p0.x;
    //        }
    //        areaP0 ^= 0b1000;
    //    }
    //    if ((areaP0 | 0b0100) == areaP0)
    //    {
    //        //在右方线框外部
    //        finalP0.x = lineP1.x;
    //        if (p1.x - p0.x != 0)
    //        {
    //            finalP0.y = (p1.y - p0.y) * (finalP0.x - p0.x) / (p1.x - p0.x) + p0.y;
    //        }
    //        areaP0 ^= 0b0100;
    //    }
    //    if ((areaP0 | 0b0010) == areaP0)
    //    {
    //        //在下方线框外部
    //        finalP0.y = lineP2.y;
    //        if (p1.y - p0.y != 0)
    //        {
    //            finalP0.x = (p1.x - p0.x) * (finalP0.y - p0.y) / (p1.y - p0.y) + p0.x;
    //        }
    //        areaP0 ^= 0b0010;
    //    }
    //    if ((areaP0 | 0b0001) == areaP0)
    //    {
    //        //在左方线框外部
    //        finalP0.x = lineP0.x;
    //        if (p1.x - p0.x != 0)
    //        {
    //            finalP0.y = (p1.y - p0.y) * (finalP0.x - p0.x) / (p1.x - p0.x) + p0.y;
    //        }
    //    }
    //}
    //else if (areaP1 != 0b0000)
    //{
    //    if ((areaP1 | 0b1000) == areaP1)
    //    {
    //        //在上方线框外部
    //        finalP1.y = lineP0.y;
    //        if (p1.y - p0.y != 0)
    //        {
    //            finalP1.x = (p1.x - p0.x) * (finalP1.y - p0.y) / (p1.y - p0.y) + p0.x;
    //        }
    //        areaP1 ^= 0b1000;
    //    }
    //    if ((areaP1 | 0b0100) == areaP1)
    //    {
    //        //在右方线框外部
    //        finalP1.x = lineP1.x;
    //        if (p1.x - p0.x != 0)
    //        {
    //            finalP1.y = (p1.y - p0.y) * (finalP1.x - p0.x) / (p1.x - p0.x) + p0.y;
    //        }
    //        areaP1 ^= 0b0100;
    //    }
    //    if ((areaP1 | 0b0010) == areaP1)
    //    {
    //        //在下方线框外部
    //        finalP1.y = lineP2.y;
    //        if (p1.y - p0.y != 0)
    //        {
    //            finalP1.x = (p1.x - p0.x) * (finalP1.y - p0.y) / (p1.y - p0.y) + p0.x;
    //        }
    //        areaP1 ^= 0b0010;
    //    }
    //    if ((areaP1 | 0b0001) == areaP1)
    //    {
    //        //在左方线框外部
    //        finalP1.x = lineP0.x;
    //        if (p1.x - p0.x != 0)
    //        {
    //            finalP1.y = (p1.y - p0.y) * (finalP1.x - p0.x) / (p1.x - p0.x) + p0.y;
    //        }
    //    }
    //}

    return vector<Point>({ finalP0, finalP1 });

}

float RandFloat()
{
    return static_cast <float> (rand() - RAND_MAX / 2) / static_cast <float> (RAND_MAX / 2);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}