#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>

typedef struct {
    Color color1;
    Color color2;
    int position; // 0 = left, 1 = right, 2 = up, 3 = down
    unsigned int occurences = 1;
} Condition;

typedef enum {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN
}Direction;

bool conditionExists(const std::vector<Condition>& conditions, const Condition& condition) {
    for (const auto& cond : conditions) {
        if (cond.color1.r == condition.color1.r &&
            cond.color1.g == condition.color1.g &&
            cond.color1.b == condition.color1.b &&
            cond.color2.r == condition.color2.r &&
            cond.color2.g == condition.color2.g &&
            cond.color2.b == condition.color2.b &&
            cond.position == condition.position) {
            return true;
        }
    }
    return false;
}

void addConditionOccurence(std::vector<Condition>& conditions, const Condition& condition) {
    for (auto& cond : conditions) {
        if (cond.color1.r == condition.color1.r &&
            cond.color1.g == condition.color1.g &&
            cond.color1.b == condition.color1.b &&
            cond.color2.r == condition.color2.r &&
            cond.color2.g == condition.color2.g &&
            cond.color2.b == condition.color2.b &&
            cond.position == condition.position) {
            cond.occurences++;
        }
    }
}

std::vector<Condition> GetConditionsFromImage(const char* fileName)
{
    Image inputImage = LoadImage(fileName);

    std::vector<Condition> conditions;
    Condition condition;

    Color leftColor;
    Color rightColor;
    Color upColor;
    Color downColor;
    Direction direction;

    for(int j = 0; j < inputImage.height; j++) {
        for(int i = 0; i < inputImage.width; i++) {
            Color pixelColor = GetImageColor(inputImage, i, j);
            if(pixelColor.r == WHITE.r && pixelColor.g == WHITE.g && pixelColor.b == WHITE.b)
            {
                // check color of all surrounding pixels
                if(i > 0)
                {
                    leftColor = GetImageColor(inputImage, i - 1, j);
                    condition = {pixelColor, leftColor, LEFT};
                    if(!conditionExists(conditions, condition))
                        conditions.push_back(condition);
                    else
                        addConditionOccurence(conditions, condition);
                }
                if(i < inputImage.width - 1)
                {
                    rightColor = GetImageColor(inputImage, i + 1, j);
                    condition = {pixelColor, rightColor, RIGHT};
                    if(!conditionExists(conditions, condition))
                        conditions.push_back(condition);
                    else
                        addConditionOccurence(conditions, condition);
                }
                if(j > 0)
                {
                    upColor = GetImageColor(inputImage, i, j - 1);
                    condition = {pixelColor, upColor, UP};
                    if(!conditionExists(conditions, condition))
                        conditions.push_back(condition);
                    else
                        addConditionOccurence(conditions, condition);
                }
                if(j < inputImage.height - 1)
                {
                    downColor = GetImageColor(inputImage, i, j + 1);
                    condition = {pixelColor, downColor, DOWN};
                    if(!conditionExists(conditions, condition))
                        conditions.push_back(condition);
                    else
                        addConditionOccurence(conditions, condition);
                }
            }
        }
    }

    UnloadImage(inputImage); // Unload the image from RAM

    return conditions;
}


int main() {
    // Initialize the window
    const int screenWidth = 128;
    const int screenHeight = 128;
    InitWindow(screenWidth, screenHeight, "Raylib Example");

    // Set the target FPS
    SetTargetFPS(60);

    std::vector<Condition> conditions = GetConditionsFromImage("src/input.png");

    // random initial position
    Vector2 initialPosition = {GetRandomValue(0,128), GetRandomValue(0,128)};

    // Main loop
    while (!WindowShouldClose()) {
        // Update logic here

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);


        // End drawing
        EndDrawing();
    }

    // Close the window and clean up resources
    CloseWindow();

    return 0;
}