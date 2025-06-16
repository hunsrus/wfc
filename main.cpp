#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>

typedef struct{
    int x;
    int y;
}Vector2i;

typedef struct {
    Color color1;
    Color color2;
    int position; // 0 = left, 1 = right, 2 = up, 3 = down
    unsigned int occurences = 1;
}Condition;

typedef enum {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN
}Direction;

typedef struct {
    bool collapsed = 0;
    double entropy = 999;
    Color color = BLACK;
    std::vector<Condition> possibleConditions;
}Cell;

void collapseCell(Cell **cell, Vector2i position, Vector2i size);

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

// find condition with the most occurences
Condition findMostCommonCondition(const std::vector<Condition>& conditions) {
    Condition mostCommonCondition = conditions[0];
    for (const auto& condition : conditions) {
        if (condition.occurences > mostCommonCondition.occurences) {
            mostCommonCondition = condition;
        }
    }
    return mostCommonCondition;
}

std::vector<Condition> getConditionsFromImage(const char* fileName)
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
            // if(pixelColor.r == WHITE.r && pixelColor.g == WHITE.g && pixelColor.b == WHITE.b)
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

double getCellEntropy(Cell **cell, Vector2i position) {
    double a = 0, b = 0;
    for(std::vector<Condition>::iterator condition = cell[position.x][position.y].possibleConditions.begin(); condition <= cell[position.x][position.y].possibleConditions.end(); condition++) {
        a += condition->occurences;
        b += condition->occurences*log(condition->occurences);
    }
    return log(a)-b/a;
}

Vector2i minimalEntropyCoords(Cell **cell, Vector2i size) {
    Vector2i min = {0,0};
    for(int j = 0; j < size.y; j++) {
        for(int i = 0; i < size.x; i++) {
            if(cell[i][j].entropy < cell[min.x][min.y].entropy && !cell[i][j].collapsed) {
                min.x = i;
                min.y = j;
            }
        }
    }
    return min;
}

bool compareColors(Color color1, Color color2)
{
    if (color1.r == color2.r &&
        color1.g == color2.g &&
        color1.b == color2.b) {
            
        return true;
    }
    else return false;
}

void propagateStates(Cell **cell, Vector2i position, Vector2i size) {
    std::vector<Condition> auxPossibleConditions;
    int i = position.x;
    int j = position.y;
    // for(int j = 0; j < size.y; j++) {
    //     for(int i = 0; i < size.x; i++) {
            if(cell[i][j].collapsed)
            {
                Color pixelColor = cell[i][j].color;
                // check color of all surrounding pixels
                if(i > 0)
                {
                    if(!cell[i-1][j].collapsed){
                        auxPossibleConditions = cell[i-1][j].possibleConditions;
                        if(auxPossibleConditions.size() == 1) {
                            // cell[i-1][j].color = auxPossibleConditions.begin()->color1;
                            // cell[i-1][j].collapsed = 1;
                            // cell[i-1][j].entropy = 999;
                            // auxPossibleConditions.clear();
                            collapseCell(cell, (Vector2i){i-1,j}, size);
                        }
                        if(!auxPossibleConditions.empty()) {
                            double a = 0, b = 0;
                            for(std::vector<Condition>::iterator condition = auxPossibleConditions.begin(); condition <= auxPossibleConditions.end(); condition++) {
                                if(condition->position == RIGHT && !compareColors(condition->color2, pixelColor)) {
                                    auxPossibleConditions.erase(condition);
                                    // condition = auxPossibleConditions.begin();
                                }else {
                                    a += condition->occurences;
                                    b += condition->occurences*log(condition->occurences);
                                }
                            }
                            cell[i-1][j].entropy = log(a)-b/a;
                        }
                        cell[i-1][j].possibleConditions = auxPossibleConditions;
                    }
                }
                if(i < size.x - 1)
                {
                    if(!cell[i+1][j].collapsed){
                        auxPossibleConditions = cell[i+1][j].possibleConditions;
                        if(auxPossibleConditions.size() == 1) {
                            // cell[i+1][j].color = auxPossibleConditions.begin()->color1;
                            // cell[i+1][j].collapsed = 1;
                            // cell[i+1][j].entropy = 999;
                            // auxPossibleConditions.clear();
                            collapseCell(cell, (Vector2i){i+1,j}, size);
                        }
                        if(!auxPossibleConditions.empty()) {
                            double a = 0, b = 0;
                            for(std::vector<Condition>::iterator condition = auxPossibleConditions.begin(); condition <= auxPossibleConditions.end(); condition++) {
                                if(condition->position == LEFT && !compareColors(condition->color2, pixelColor)) {
                                    auxPossibleConditions.erase(condition);
                                    // condition = auxPossibleConditions.begin();
                                }else {
                                    a += condition->occurences;
                                    b += condition->occurences*log(condition->occurences);
                                }
                            }
                            cell[i+1][j].entropy = log(a)-b/a;
                        }
                        cell[i+1][j].possibleConditions = auxPossibleConditions;
                    }
                }
                if(j > 0)
                {
                    if(!cell[i][j-1].collapsed){
                        auxPossibleConditions = cell[i][j-1].possibleConditions;
                        if(auxPossibleConditions.size() == 1) {
                            // cell[i][j-1].color = auxPossibleConditions.begin()->color1;
                            // cell[i][j-1].collapsed = 1;
                            // cell[i][j-1].entropy = 999;
                            // auxPossibleConditions.clear();
                            collapseCell(cell, (Vector2i){i,j-1}, size);
                        }
                        if(!auxPossibleConditions.empty()) {
                            double a = 0, b = 0;
                            for(std::vector<Condition>::iterator condition = auxPossibleConditions.begin(); condition <= auxPossibleConditions.end(); condition++) {
                                if(condition->position == UP && !compareColors(condition->color2, pixelColor)) {
                                    auxPossibleConditions.erase(condition);
                                    // condition = auxPossibleConditions.begin();
                                }else {
                                    a += condition->occurences;
                                    b += condition->occurences*log(condition->occurences);
                                }
                            }
                            cell[i][j-1].entropy = log(a)-b/a;
                        }
                        cell[i][j-1].possibleConditions = auxPossibleConditions;
                    }
                }
                if(j < size.y - 1)
                {
                    if(!cell[i][j+1].collapsed){
                        auxPossibleConditions = cell[i][j+1].possibleConditions;
                        if(auxPossibleConditions.size() == 1) {
                            // cell[i][j+1].color = auxPossibleConditions.begin()->color1;
                            // cell[i][j+1].collapsed = 1;
                            // cell[i][j+1].entropy = 999;
                            // auxPossibleConditions.clear();
                            collapseCell(cell, (Vector2i){i,j+1}, size);
                        }
                        if(!auxPossibleConditions.empty()) {
                            double a = 0, b = 0;
                            for(std::vector<Condition>::iterator condition = auxPossibleConditions.begin(); condition <= auxPossibleConditions.end(); condition++) {
                                if(condition->position == DOWN && !compareColors(condition->color2, pixelColor)) {
                                    auxPossibleConditions.erase(condition);
                                    // condition = auxPossibleConditions.begin();
                                }else {
                                    a += condition->occurences;
                                    b += condition->occurences*log(condition->occurences);
                                }
                            }
                            cell[i][j+1].entropy = log(a)-b/a;
                        }
                        cell[i][j+1].possibleConditions = auxPossibleConditions;
                    }
                }
            }
    //     }
    // }
}

void collapseCell(Cell **cell, Vector2i position, Vector2i size) {

    int conditionIndex = GetRandomValue(0,cell[position.x][position.y].possibleConditions.size()-1);
    cell[position.x][position.y].color = cell[position.x][position.y].possibleConditions.at(conditionIndex).color1;
    cell[position.x][position.y].collapsed = 1;
    cell[position.x][position.y].entropy = 0;
    cell[position.x][position.y].possibleConditions.clear();
    
    propagateStates(cell, position, size);
}

int main() {
    // Initialize the window
    const int screenWidth = 480;
    const int screenHeight = 480;
    InitWindow(screenWidth, screenHeight, "Raylib Example");

    // Set the target FPS
    SetTargetFPS(60);

    std::vector<Condition> conditions = getConditionsFromImage("src/input3.png");
    
    Vector2i mapSize = {12, 12};

    // random initial position
    Vector2i initialPosition = {GetRandomValue(0,mapSize.x-1), GetRandomValue(0,mapSize.y-1)};

    // Cell** map;
    // map = (Cell**)malloc(mapSize.x * sizeof(Cell*));
    // for(int i = 0; i < mapSize.x; i++) {
    //     map[i] = (Cell*)malloc(mapSize.y * sizeof(Cell));
    //     for(int j = 0; j < mapSize.y; j++){
            
    //     }
    // }

    Cell** map = new Cell*[mapSize.x];
    for(int i = 0; i < mapSize.x; i++) {
        map[i] = new Cell[mapSize.y];
    }

    for(int i = 0; i < mapSize.x; i++)
        for(int j = 0; j < mapSize.y; j++){
            map[i][j].collapsed = 0;
            map[i][j].color = BLACK;
            map[i][j].entropy = 0;
            map[i][j].possibleConditions = conditions;
        }

    collapseCell(map, initialPosition, mapSize);

    // Main loop
    while (!WindowShouldClose()) {
        // Update logic here
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            collapseCell(map, (Vector2i){GetMouseX()/10, GetMouseY()/10}, mapSize);
        }
        // if(IsKeyDown(KEY_SPACE)) {
            // initialPosition = {GetRandomValue(0,mapSize.x-1), GetRandomValue(0,mapSize.y-1)};
            initialPosition = minimalEntropyCoords(map,mapSize);
            collapseCell(map, initialPosition, mapSize);
        // }

        // Start drawing
        BeginDrawing();
        ClearBackground(RED);

            for(int i = 0; i < mapSize.x; i++){
                for(int j = 0; j < mapSize.y; j++){
                    DrawRectangle(i*10-5,j*10-5,10,10,map[i][j].color);
                    // DrawRectangle(i*10-5,j*10-5,10,10,(Color){map[i][j].entropy*128,0,0,255});
                    // DrawPixel(i,j,map[i][j].color);
                }
            }


        // End drawing
        EndDrawing();
    }

    // Close the window and clean up resources
    CloseWindow();

    for(int i = 0; i < mapSize.x; i++) {
        free(map[i]);
    }
    free(map);

    return 0;
}