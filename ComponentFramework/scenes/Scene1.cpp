#include "Scene1.h"

#include <SDL3/SDL_events.h>


#include "../MaterialComponent.h"
#include "../MeshComponent.h"
#include "../ShaderComponent.h"
#include "../TransformComponent.h"

Scene1::Scene1()
{
}


void Scene1::MoveActorBy(const Actor* actor, const MATH::Vec3& delta)
{
    TransformComponent* transform = actor->GetComponent<TransformComponent>();
    const Vec3 position = transform->GetPosition();

    transform->SetTransform(
        Vec3(position.x + delta.x, position.y + delta.y, position.z + delta.z),
        transform->GetQuaternion(),
        transform->GetScale()
    );
}

std::vector<int> Scene1::GetColPositionListByPiece(const chess_pieces pieceName)
{
    switch (pieceName)
    {
    case chess_pieces::PAWN:
        return {0, 1, 2, 3, 4, 5, 6, 7};
    case chess_pieces::KNIGHT:
        return {1, 6};
    case chess_pieces::BISHOP:
        return {2, 5};
    case chess_pieces::ROOK:
        return {0, 7};
    case chess_pieces::QUEEN:
        return {3};
    case chess_pieces::KING:
        return {4};
    default:
        return {};
    }
}

const char* Scene1::GetMeshNameByPiece(const chess_pieces pieceName)
{
    switch (pieceName)
    {
    case chess_pieces::PAWN:
        return "meshes/Pawn.obj";
    case chess_pieces::KNIGHT:
        return "meshes/Knight.obj";
    case chess_pieces::BISHOP:
        return "meshes/Bishop.obj";
    case chess_pieces::ROOK:
        return "meshes/Rook.obj";
    case chess_pieces::QUEEN:
        return "meshes/Queen.obj";
    case chess_pieces::KING:
        return "meshes/King.obj";
    }
    return "meshes/Pawn.obj";
}

Vec3 Scene1::GetRelativeTransformOnBoard(int row, int col)
{
    float cellSize = 1.25f;
    /** Move to 0,0 **/
    float xPositionOnBoard = -cellSize * 3.0f - cellSize / 2.0f;
    float yPositionOnBoard = -cellSize * 3.0f - cellSize / 2.0f;
    /** Move to cell **/
    xPositionOnBoard += cellSize * (static_cast<float>(col));
    yPositionOnBoard += cellSize * (static_cast<float>(row));

    return {xPositionOnBoard, yPositionOnBoard, 0.0f};
}

bool Scene1::OnCreate()
{
    camera = new CameraActor(nullptr, 45.0f, 16.0f / 9.0f, 0.5f, 1000.0f);
    camera->AddComponent<TransformComponent>(nullptr, Vec3(1.5f, -1.0f, -10.0f), Quaternion());
    camera->OnCreate();

    /** Create Board **/
    board = new Actor(nullptr);
    board->AddComponent<MeshComponent>(nullptr, "meshes/Plane.obj");
    board->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    board->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.5f, 0.0f),
                                            QMath::angleAxisRotation(
                                                -90.0f, Vec3(1.0f, 0.0f, 0.0f)),
                                            Vec3(1.5f, 1.5f, 1.5f));
    board->AddComponent<MaterialComponent>(nullptr, "textures/8x8_checkered_board.png");
    board->OnCreate();
    board->ListComponents();

    /** Initialize Meshes **/
    for (auto mesh : {
             chess_pieces::KING, chess_pieces::PAWN, chess_pieces::ROOK, chess_pieces::QUEEN, chess_pieces::BISHOP,
             chess_pieces::KNIGHT
         })
    {
        auto meshComponent =
            std::make_shared<MeshComponent>(nullptr, GetMeshNameByPiece(mesh));
        chessPieceMeshes.insert({chess_pieces::KING, meshComponent});
    }

    int index = 0;
    for (auto const& color : {"textures/White Chess Base Colour.png", "textures/Black Chess Base Colour.png"})
    {
        for (const auto chessPiece : {
                 chess_pieces::KING, chess_pieces::PAWN, chess_pieces::ROOK, chess_pieces::QUEEN, chess_pieces::BISHOP,
                 chess_pieces::KNIGHT
             })
        {
            for (auto colPosition : GetColPositionListByPiece(chessPiece))
            {
                auto actor = new Actor(board);
                actor->AddComponent<MeshComponent>(nullptr, GetMeshNameByPiece(chessPiece));
                actor->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl",
                                                     "shaders/texturePhongFrag.glsl");
                auto rotationByColor = index
                                           ? QMath::angleAxisRotation(
                                               90.0f, Vec3(1.0f, 0.0f, 0.0f))
                                           : QMath::angleAxisRotation(
                                               90.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(
                                               180.0f, Vec3(0.0f, 1.0f, 0.0f));
                actor->AddComponent<TransformComponent>(
                    nullptr, Vec3(
                        GetRelativeTransformOnBoard(chessPiece == chess_pieces::PAWN ? index * 5 + 1 : index * 7, colPosition)),
                    rotationByColor
                    ,
                    Vec3(0.15f, 0.15f, 0.15f));
                actor->AddComponent<MaterialComponent>(nullptr, color);
                actor->OnCreate();
                actor->ListComponents();
                pieces.push_back(actor);
            }
        }
        index++;
    }


    return true;
}

void Scene1::OnDestroy()
{
}

void Scene1::HandleEvents(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.type)
    {
    case SDL_EVENT_KEY_DOWN:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_A: MoveActorBy(board, Vec3(-0.25f, 0.0f, 0.0f));
            break;
        case SDL_SCANCODE_D: MoveActorBy(board, Vec3(0.25f, 0.0f, 0.0f));
            break;
        case SDL_SCANCODE_W: MoveActorBy(board, Vec3(0.0f, 0.0f, -0.25f));
            break;
        case SDL_SCANCODE_S: MoveActorBy(board, Vec3(0.0f, 0.0f, 0.25f));
            break;
        default: break;
        }
    }
}

void Scene1::Update(float deltaTime)
{
    /** Camera movement **/
    const bool* keyboardState = SDL_GetKeyboardState(NULL);
    Vec3 velocity(0.0f, 0.0f, 0.0f);
    float CameraSpeed = 20.0f;
    if (keyboardState[SDL_SCANCODE_W]) velocity.z -= CameraSpeed;
    if (keyboardState[SDL_SCANCODE_S]) velocity.z += CameraSpeed;
    if (keyboardState[SDL_SCANCODE_A]) velocity.x -= CameraSpeed;
    if (keyboardState[SDL_SCANCODE_D]) velocity.x += CameraSpeed;
    if (keyboardState[SDL_SCANCODE_SPACE]) velocity.y += CameraSpeed;
    if (keyboardState[SDL_SCANCODE_LSHIFT]) velocity.y -= CameraSpeed;
    if (VMath::mag(velocity) > 0.0f)
    {
        velocity = VMath::normalize(velocity);
        Vec3 displacement = velocity * CameraSpeed * deltaTime;
        camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(displacement));
    }
}

void Scene1::Render() const
{
    glClearColor(0.12f, 0.72f, 0.52f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    ShaderComponent* shader = board->GetComponent<ShaderComponent>();

    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                       camera->GetProjectionMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera->GetViewMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                       board->GetComponent<TransformComponent>()->GetTransformMatrix());
    glBindTexture(GL_TEXTURE_2D, board->GetComponent<MaterialComponent>()->getTextureID());
    board->GetComponent<MeshComponent>()->Render();

    /** Render all Pieces **/
    for (const auto piece : pieces)
    {
        glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                           piece->GetModelMatrix());
        glBindTexture(GL_TEXTURE_2D, piece->GetComponent<MaterialComponent>()->getTextureID());
        piece->GetComponent<MeshComponent>()->Render();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
