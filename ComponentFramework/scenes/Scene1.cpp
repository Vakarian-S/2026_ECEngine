#include "Scene1.h"

#include <SDL3/SDL_events.h>


#include "../MaterialComponent.h"
#include "../MeshComponent.h"
#include "../ShaderComponent.h"
#include "../TransformComponent.h"

Scene1::Scene1()
{
}


void Scene1::MoveActorBy(const Actor* actor, const Vec3& delta)
{
    const Ref transform = actor->GetComponent<TransformComponent>();
    const Vec3 position = transform->GetPosition();

    transform->SetTransform(
        Vec3(position.x + delta.x, position.y + delta.y, position.z + delta.z),
        transform->GetQuaternion(),
        transform->GetScale()
    );
}

std::vector<int> Scene1::GetColPositionListByPiece(const Chess_pieces pieceName)
{
    switch (pieceName)
    {
    case Chess_pieces::PAWN:
        return {0, 1, 2, 3, 4, 5, 6, 7};
    case Chess_pieces::KNIGHT:
        return {1, 6};
    case Chess_pieces::BISHOP:
        return {2, 5};
    case Chess_pieces::ROOK:
        return {0, 7};
    case Chess_pieces::QUEEN:
        return {3};
    case Chess_pieces::KING:
        return {4};
    default:
        return {};
    }
}


Vec3 Scene1::GetRelativeTransformOnBoard(int row, int col)
{
    constexpr float cellSize = 1.25f;
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
    /** Setup Camera **/
    camera_ = new CameraActor(nullptr, 45.0f, 16.0f / 9.0f, 0.5f, 1000.0f);
    camera_->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 2.0f, 15.0f), Quaternion());
    camera_->OnCreate();

    /** Create Board **/
    board_ = std::make_shared<Actor>(nullptr);
    board_->AddComponent<MeshComponent>(nullptr, "meshes/Plane.obj");
    board_->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    board_->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.5f, 0.0f),
                                             QMath::angleAxisRotation(
                                                 -90.0f, Vec3(1.0f, 0.0f, 0.0f)),
                                             Vec3(1.5f, 1.5f, 1.5f));
    base_board_orientation_quaternion_ = QMath::angleAxisRotation(
        -90.0f, Vec3(1.0f, 0.0f, 0.0f));
    board_->AddComponent<MaterialComponent>(nullptr, "textures/8x8_checkered_board.png");
    board_->OnCreate();
    board_->ListComponents();


    auto light1 = std::make_shared<LightActor>(nullptr);
    light1->AddComponent<TransformComponent>(nullptr, Vec3(5.0f, 0.0f, 0.0f),
                                             Quaternion(),
                                             Vec3(0.5f, 0.5f, 0.5f));
    light1->AddComponent<MeshComponent>(nullptr, "meshes/Sphere.obj");
    light1->AddComponent<
        ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    light1->AddComponent<MaterialComponent>(nullptr, "textures/8x8_checkered_board.png");
    light1->SetDiffuseLightColor(Vec3(0.8f, 0.0f, 0.4f));
    light1->SetSpecularLightColor(Vec3(1.0f, 0.0f, 0.1f));
    light1->OnCreate();
    light1->ListComponents();

    auto light2 = std::make_shared<LightActor>(nullptr);
    light2->AddComponent<TransformComponent>(nullptr, Vec3(-5.0f, 0.0f, 0.0f),
                                             Quaternion(),
                                             Vec3(0.5f, 0.5f, 0.5f));
    light2->AddComponent<MeshComponent>(nullptr, "meshes/Sphere.obj");
    light2->AddComponent<
        ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    light2->AddComponent<MaterialComponent>(nullptr, "textures/8x8_checkered_board.png");
    light2->SetDiffuseLightColor(Vec3(0.3f, 0.0f, 0.8f));
    light2->SetSpecularLightColor(Vec3(0.1f, 0.0f, 1.0f));
    light2->OnCreate();
    light2->ListComponents();

    point_lights_.push_back(light1);
    point_lights_.push_back(light2);

    /** Setup Mesh Filenames **/
    mesh_filenames_ = {
        {Chess_pieces::BISHOP, "meshes/Bishop.obj"},
        {Chess_pieces::KING, "meshes/King.obj"},
        {Chess_pieces::KNIGHT, "meshes/Knight.obj"},
        {Chess_pieces::PAWN, "meshes/Pawn.obj"},
        {Chess_pieces::QUEEN, "meshes/Queen.obj"},
        {Chess_pieces::ROOK, "meshes/Rook.obj"}
    };

    /** Make a component for each Mesh **/
    for (auto const& chessPiece : {
             Chess_pieces::KING, Chess_pieces::PAWN, Chess_pieces::ROOK, Chess_pieces::QUEEN, Chess_pieces::BISHOP,
             Chess_pieces::KNIGHT
         })
    {
        const auto meshActor = std::make_shared<MeshComponent>(nullptr, mesh_filenames_[chessPiece].c_str());
        chess_piece_meshes_[chessPiece] = meshActor;
    }

    /** Setup the Pieces on the board Iteration Galore **/
    int index = 0;
    for (auto const& color : {"textures/White Chess Base Colour.png", "textures/Black Chess Base Colour.png"})
    {
        for (const auto chessPiece : {
                 Chess_pieces::KING, Chess_pieces::PAWN, Chess_pieces::ROOK, Chess_pieces::QUEEN, Chess_pieces::BISHOP,
                 Chess_pieces::KNIGHT
             })
        {
            for (const auto colPosition : GetColPositionListByPiece(chessPiece))
            {
                auto actor = std::make_shared<Actor>(board_);
                Ref<MeshComponent> mesh = chess_piece_meshes_[chessPiece];
                actor->AddComponent<MeshComponent>(chess_piece_meshes_[chessPiece]);
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
                        GetRelativeTransformOnBoard(chessPiece == Chess_pieces::PAWN ? index * 5 + 1 : index * 7,
                                                    colPosition)),
                    rotationByColor
                    ,
                    Vec3(0.15f, 0.15f, 0.15f));
                actor->AddComponent<MaterialComponent>(nullptr, color);
                actor->OnCreate();
                actor->ListComponents();
                chess_piece_actors_.push_back(actor);
            }
        }
        index++;
    }

    return true;
}

void Scene1::OnDestroy()
{
    chess_piece_actors_.clear();
    chess_piece_meshes_.clear();
}

void Scene1::HandleEvents(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.type)
    {
    case SDL_EVENT_KEY_DOWN:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_A: MoveActorBy(board_.get(), Vec3(-0.25f, 0.0f, 0.0f));
            break;
        case SDL_SCANCODE_D: MoveActorBy(board_.get(), Vec3(0.25f, 0.0f, 0.0f));
            break;
        case SDL_SCANCODE_W: MoveActorBy(board_.get(), Vec3(0.0f, 0.0f, -0.25f));
            break;
        case SDL_SCANCODE_S: MoveActorBy(board_.get(), Vec3(0.0f, 0.0f, 0.25f));
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
        camera_->SetView(camera_->GetOrientation(), camera_->freeCameraMovement(displacement));
    }

    /** Rotate the Board using slerp because why not **/

    static float totalTime = 0.0f;
    totalTime += deltaTime;

    Quaternion leftRotation = QMath::angleAxisRotation(-45.0f, Vec3(0.0f, 1.0f, 0.0f));
    Quaternion rightRotation = QMath::angleAxisRotation(45.0f, Vec3(0.0f, 1.0f, 0.0f));

    float speed = 1.0f;
    float t = (sin(totalTime * speed) + 1.0f) / 2.0f;

    Quaternion newRotation = QMath::slerp(leftRotation, rightRotation, t);

    board_->GetComponent<TransformComponent>()->SetQuaternion(newRotation);

    static float totalTimeSeconds = 0.0f;
    totalTimeSeconds += deltaTime;

    float animationSpeedMultiplier = 1.0f;
    float interpolationParameter = (sin(totalTimeSeconds * animationSpeedMultiplier) + 1.0f) / 2.0f;

    Quaternion leftOffsetRotationQuaternion =
        QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f));

    Quaternion rightOffsetRotationQuaternion =
        QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f));

    Quaternion animatedOffsetRotationQuaternion =
        QMath::slerp(leftOffsetRotationQuaternion, rightOffsetRotationQuaternion, interpolationParameter);

    Quaternion finalBoardOrientationQuaternion =
        animatedOffsetRotationQuaternion * base_board_orientation_quaternion_;

    board_->GetComponent<TransformComponent>()->SetQuaternion(finalBoardOrientationQuaternion);

    /** Moving Lights up and down just to look at them go **/

    if (point_lights_[0]->GetComponent<TransformComponent>()->GetPosition().y > 10.0f)
    {
        going_up_ = false;
    }
    if (point_lights_[0]->GetComponent<TransformComponent>()->GetPosition().y < -1.0f)
    {
        going_up_ = true;
    }

    for (auto pointLightItem : point_lights_)
    {
        auto lightTransform = pointLightItem->GetComponent<TransformComponent>();
        auto sign = going_up_ ? 1.0f : -1.0f;
        auto newPosition = Vec3(
            lightTransform->GetPosition().x,
            lightTransform->GetPosition().y + sign * deltaTime * 10.0f,
            lightTransform->GetPosition().z
        );

        lightTransform->SetTransform(
            newPosition,
            lightTransform->GetQuaternion(),
            lightTransform->GetScale()
        );
    }
}

namespace
{
    void UploadPointLightsToShader(
        Ref<ShaderComponent> shader,
        const std::vector<Ref<LightActor>>& pointLightActorList
    )
    {
        if (shader == nullptr)
        {
            return;
        }

        const int requestedPointLightCount = static_cast<int>(pointLightActorList.size());
        const int clampedPointLightCount = std::max(0, std::min(requestedPointLightCount, 8));

        /** If there are no lights, still upload the count so the shader loop does not run. **/
        glUniform1i(
            static_cast<GLint>(shader->GetUniformID("activePointLightCount")),
            clampedPointLightCount
        );


        if (clampedPointLightCount == 0)
        {
            return;
        }

        std::vector<float> pointLightWorldPositionPackedArray;
        std::vector<float> pointLightDiffuseLightColorPackedArray;
        std::vector<float> pointLightSpecularLightColorPackedArray;

        std::vector<float> pointLightIntensityMultiplierPackedArray;
        std::vector<float> pointLightAttenuationConstantPackedArray;
        std::vector<float> pointLightAttenuationLinearPackedArray;
        std::vector<float> pointLightAttenuationQuadraticPackedArray;

        /** Packing Arrays for 3 elements by pre-allocating the memory **/
        pointLightWorldPositionPackedArray.reserve(static_cast<size_t>(clampedPointLightCount) * 3u);
        pointLightDiffuseLightColorPackedArray.reserve(static_cast<size_t>(clampedPointLightCount) * 3u);
        pointLightSpecularLightColorPackedArray.reserve(static_cast<size_t>(clampedPointLightCount) * 3u);

        pointLightIntensityMultiplierPackedArray.reserve(static_cast<size_t>(clampedPointLightCount));
        pointLightAttenuationConstantPackedArray.reserve(static_cast<size_t>(clampedPointLightCount));
        pointLightAttenuationLinearPackedArray.reserve(static_cast<size_t>(clampedPointLightCount));
        pointLightAttenuationQuadraticPackedArray.reserve(static_cast<size_t>(clampedPointLightCount));

        /**  Iterate through max lights, not all the point lights you have **/
        for (int pointLightIndex = 0; pointLightIndex < clampedPointLightCount; pointLightIndex++)
        {
            const auto& pointLightActor = pointLightActorList[pointLightIndex];
            if (!pointLightActor || pointLightActor.get() == nullptr)
            {
                // Fill with safe defaults if a null sneaks in.
                pointLightWorldPositionPackedArray.insert(pointLightWorldPositionPackedArray.end(), {0.0f, 0.0f, 0.0f});
                pointLightDiffuseLightColorPackedArray.insert(pointLightDiffuseLightColorPackedArray.end(),
                                                              {0.0f, 0.0f, 0.0f});
                pointLightSpecularLightColorPackedArray.insert(pointLightSpecularLightColorPackedArray.end(),
                                                               {0.0f, 0.0f, 0.0f});

                pointLightIntensityMultiplierPackedArray.push_back(0.0f);
                pointLightAttenuationConstantPackedArray.push_back(1.0f);
                pointLightAttenuationLinearPackedArray.push_back(0.0f);
                pointLightAttenuationQuadraticPackedArray.push_back(0.0f);
                continue;
            }

            const Ref transformComponent = pointLightActor->GetComponent<TransformComponent>();
            const Vec3 pointLightWorldPosition = transformComponent->GetPosition();

            const PointLightParameters& pointLightParameters = pointLightActor->GetPointLightParameters();

            pointLightWorldPositionPackedArray.push_back(pointLightWorldPosition.x);
            pointLightWorldPositionPackedArray.push_back(pointLightWorldPosition.y);
            pointLightWorldPositionPackedArray.push_back(pointLightWorldPosition.z);

            pointLightDiffuseLightColorPackedArray.push_back(pointLightParameters.diffuseLightColor.x);
            pointLightDiffuseLightColorPackedArray.push_back(pointLightParameters.diffuseLightColor.y);
            pointLightDiffuseLightColorPackedArray.push_back(pointLightParameters.diffuseLightColor.z);

            pointLightSpecularLightColorPackedArray.push_back(pointLightParameters.specularLightColor.x);
            pointLightSpecularLightColorPackedArray.push_back(pointLightParameters.specularLightColor.y);
            pointLightSpecularLightColorPackedArray.push_back(pointLightParameters.specularLightColor.z);

            pointLightIntensityMultiplierPackedArray.push_back(pointLightParameters.lightIntensityMultiplier);
            pointLightAttenuationConstantPackedArray.push_back(pointLightParameters.attenuationConstant);
            pointLightAttenuationLinearPackedArray.push_back(pointLightParameters.attenuationLinear);
            pointLightAttenuationQuadraticPackedArray.push_back(pointLightParameters.attenuationQuadratic);
        }

        glUniform3fv(
            static_cast<GLint>(shader->GetUniformID("pointLightWorldPositionArray[0]")),
            clampedPointLightCount,
            pointLightWorldPositionPackedArray.data()
        );

        glUniform3fv(
            static_cast<GLint>(shader->GetUniformID("pointLightDiffuseLightColorArray[0]")),
            clampedPointLightCount,
            pointLightDiffuseLightColorPackedArray.data()
        );

        glUniform3fv(
            static_cast<GLint>(shader->GetUniformID("pointLightSpecularLightColorArray[0]")),
            clampedPointLightCount,
            pointLightSpecularLightColorPackedArray.data()
        );

        glUniform1fv(
            static_cast<GLint>(shader->GetUniformID("pointLightIntensityMultiplierArray[0]")),
            clampedPointLightCount,
            pointLightIntensityMultiplierPackedArray.data()
        );

        glUniform1fv(
            static_cast<GLint>(shader->GetUniformID("pointLightAttenuationConstantArray[0]")),
            clampedPointLightCount,
            pointLightAttenuationConstantPackedArray.data()
        );

        glUniform1fv(
            static_cast<GLint>(shader->GetUniformID("pointLightAttenuationLinearArray[0]")),
            clampedPointLightCount,
            pointLightAttenuationLinearPackedArray.data()
        );

        glUniform1fv(
            static_cast<GLint>(shader->GetUniformID("pointLightAttenuationQuadraticArray[0]")),
            clampedPointLightCount,
            pointLightAttenuationQuadraticPackedArray.data()
        );
        glUniform1f(static_cast<GLint>(shader->GetUniformID("specularShininessExponent")), 14.0f);
    }
}

void Scene1::Render() const
{
    glClearColor(0.02f, 0.00f, 0.05f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    const Ref<ShaderComponent> shader = board_->GetComponent<ShaderComponent>();
    auto myId = shader->GetProgram();


    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                       camera_->GetProjectionMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera_->GetViewMatrix());
    glUniform4fv(static_cast<GLint>(shader->GetUniformID("ambientLightColor")), 1,
                 Vec4(1.0f, 0.0f, 0.5f, 0.0f));


    glUniform3fv(static_cast<GLint>(shader->GetUniformID("cameraWorldPosition")), 1,
                 camera_->GetComponent<TransformComponent>()->GetPosition());

    /** Render Point Light Models **/
    for (auto pointLightItem : point_lights_)
    {
        glUniform4fv(static_cast<GLint>(shader->GetUniformID("ambientLightColor")), 1,
                     pointLightItem->GetPointLightParameters().specularLightColor);
        glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                           pointLightItem->GetModelMatrix());
        glBindTexture(GL_TEXTURE_2D, pointLightItem->GetComponent<MaterialComponent>()->getTextureID());
        pointLightItem->GetComponent<MeshComponent>()->Render();
    }

    /** Rendering Point Lights **/
    UploadPointLightsToShader(shader, point_lights_);
    /** End Point Lights **/


    glUniform4fv(static_cast<GLint>(shader->GetUniformID("ambientLightColor")), 1,
                 Vec4(0.02f, 0.00f, 0.05f, 0.0f));
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                       board_->GetComponent<TransformComponent>()->GetTransformMatrix());

    glBindTexture(GL_TEXTURE_2D, board_->GetComponent<MaterialComponent>()->getTextureID());
    board_->GetComponent<MeshComponent>()->Render();

    /** Render all Pieces **/
    /** Use A reference "&" to avoid copying the pieces **/
    for (const Ref<Actor>& piece : chess_piece_actors_)
    {
        glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                           piece->GetModelMatrix());
        glBindTexture(GL_TEXTURE_2D, piece->GetComponent<MaterialComponent>()->getTextureID());
        piece->GetComponent<MeshComponent>()->Render();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
