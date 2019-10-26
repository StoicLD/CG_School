#include "Ld.h"
static std::vector<float> Bresenham(int x0, int y0, int x1, int y1)
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
static std::vector<float> DDA(int x0, int y0, int x1, int y1)
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
