#include "Scene1.h"

#include <SDL3/SDL_events.h>
#include <algorithm>
#include <cmath>
#include <chrono>

#include "../MaterialComponent.h"
#include "../MeshComponent.h"
#include "../ShaderComponent.h"
#include "../TransformComponent.h"
#include "../MemoryDiagnostics.h"


Scene1::Scene1() : camera_(nullptr)
{
    /** Generate a random seed on every construction so that everytime it is different **/
    const auto seed = static_cast<uint32_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    );
    fireworks_random_seed_ = std::mt19937(seed);
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
    }
    return {-1};
}


Vec3 Scene1::GetRelativeTransformOnBoard(const int row, const int col)
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
    camera_ = std::make_unique<CameraActor>(nullptr, 45.0f, 16.0f / 9.0f, 0.5f, 1000.0f);
    camera_->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 2.0f, 25.0f), Quaternion());
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

    /** Create Static Light **/
    auto ambientPointLight = std::make_shared<LightActor>(nullptr);
    ambientPointLight->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 15.0f, 0.0f),
                                                        Quaternion(),
                                                        Vec3(0.5f, 0.5f, 0.5f));
    ambientPointLight->AddComponent<MeshComponent>(nullptr, "meshes/Sphere.obj");
    ambientPointLight->AddComponent<
        ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    ambientPointLight->AddComponent<MaterialComponent>(nullptr, "textures/white_texture.png");
    ambientPointLight->SetDiffuseLightColor(Vec3(0.0f, 0.0f, 1.0f));
    ambientPointLight->SetSpecularLightColor(Vec3(0.0f, 0.0f, 1.0f));
    ambientPointLight->SetLightIntensityMultiplier(0.5f);
    ambientPointLight->SetAttenuationParameters(1.0f, 0.01f, 0.001f);
    ambientPointLight->OnCreate();
    ambientPointLight->ListComponents();

    all_lights_.emplace_back(ambientPointLight);
    point_lights_.emplace_back(std::move(ambientPointLight));
    static_point_light_count_ = point_lights_.size();

    /** Create Dynamic Lights **/
    constexpr size_t shootingStarCount = 5;
    fireworks_.reserve(shootingStarCount);
    for (size_t i = 0; i < shootingStarCount; ++i)
    {
        auto star = std::make_shared<Firework>();
        star->light_actor = std::make_unique<LightActor>(nullptr);
        star->light_actor->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 4.0f, 0.0f), Quaternion(),
                                                            Vec3(0.05f, 0.05f, 0.05f));
        star->light_actor->AddComponent<MeshComponent>(nullptr, "meshes/Sphere.obj");
        star->light_actor->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl",
                                                         "shaders/texturePhongFrag.glsl");
        star->light_actor->AddComponent<MaterialComponent>(nullptr, "textures/white_texture.png");
        star->light_actor->OnCreate();
        SpawnFirework(*star);
        all_lights_.emplace_back(star->light_actor);
        fireworks_.emplace_back(std::move(star));
    }


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

#ifdef _DEBUG
    ReferenceCountCheck(board_, chess_piece_actors_, point_lights_, fireworks_, "Scene1", Check_context::POST_CREATION);
#endif

    return true;
}

void Scene1::OnDestroy()
{
#ifdef _DEBUG
    std::cout << "\n[Scene1::OnDestroy] Starting scene destruction..." << '\n';
    ReferenceCountCheck(board_, chess_piece_actors_, point_lights_, fireworks_, "Scene1", Check_context::PRE_DESTRUCTION);
#endif

    chess_piece_actors_.clear();
    chess_piece_meshes_.clear();
    point_lights_.clear();
    fireworks_.clear();
    static_point_light_count_ = 0;

#ifdef _DEBUG
    std::cout <<
        "[Scene1::OnDestroy] Vectors cleared. Remaining references will be destroyed when board_ goes out of scope." <<
        '\n';
#endif
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
    default:
        break;
    }
}

void Scene1::Update(float deltaTime)
{
    /** Camera movement **/
    const bool* keyboardState = SDL_GetKeyboardState(nullptr);
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

    float animationSpeedMultiplier = 0.01f;
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
    UpdateFireworks(deltaTime);
}

void Scene1::SpawnFirework(Firework& firework) const
{
    /** In here we use distributions to randomize the firework parameters each time it spawns **/
    /** Spawn position: random XZ on ground level, Y near base **/
    std::uniform_real_distribution<float> spawnX(-10.0f, 10.0f);
    std::uniform_real_distribution<float> spawnZ(-10.0f, 10.0f);

    /** Upward velocity with slight horizontal drift  **/
    std::uniform_real_distribution<float> upSpeed(2.0f, 7.0f);
    std::uniform_real_distribution<float> driftX(-0.5f, 0.5f);
    std::uniform_real_distribution<float> driftZ(-0.5f, 0.5f);

    /** Ascent & explosion timing **/
    std::uniform_real_distribution<float> ascentTime(3.0f, 6.0f);
    constexpr float explosionDuration = 2.0f;

    /** Attenuation **/
    std::uniform_real_distribution<float> linearStart(0.010f, 0.040f);
    std::uniform_real_distribution<float> linearEnd(0.15f, 0.45f);
    std::uniform_real_distribution<float> quadraticStart(0.001f, 0.008f);
    std::uniform_real_distribution<float> quadraticEnd(0.03f, 0.12f);

    /** Color **/
    std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);

    /** Initial age offset to stagger fireworks **/
    std::uniform_real_distribution<float> initialAge(0.0f, 4.0f);

    /** Set all start parameters with the random values we generated **/
    firework.age_seconds = initialAge(fireworks_random_seed_);
    firework.ascent_duration = ascentTime(fireworks_random_seed_);
    firework.explosion_start = firework.ascent_duration;
    firework.has_exploded = false;
    firework.lifetime_seconds = firework.ascent_duration + explosionDuration;
    firework.base_intensity = 2.0f;
    firework.explosion_intensity_peak = 10.0f;
    firework.flicker_speed = 10.0f;
    firework.flicker_amplitude = 1.0f;

    firework.attenuation_linear_start = linearStart(fireworks_random_seed_);
    firework.attenuation_linear_end = linearEnd(fireworks_random_seed_);
    firework.attenuation_quadratic_start = quadraticStart(fireworks_random_seed_);
    firework.attenuation_quadratic_end = quadraticEnd(fireworks_random_seed_);

    const Vec3 newPosition(spawnX(fireworks_random_seed_), -0.5f, spawnZ(fireworks_random_seed_));
    const float upVelocity = upSpeed(fireworks_random_seed_);
    firework.velocity = Vec3(driftX(fireworks_random_seed_), upVelocity, driftZ(fireworks_random_seed_));

    const Ref transform = firework.light_actor->GetComponent<TransformComponent>();
    transform->SetTransform(newPosition, transform->GetQuaternion(), transform->GetScale());

    /** Color Setup on the Light Actor Itself **/
    const auto randomizedColor =
        Vec3(randomColor(fireworks_random_seed_), randomColor(fireworks_random_seed_),
             randomColor(fireworks_random_seed_));
    firework.light_actor->SetDiffuseLightColor(randomizedColor);
    firework.light_actor->SetSpecularLightColor(randomizedColor * 1.5f);
    firework.light_actor->SetLightIntensityMultiplier(0.0f);
    firework.light_actor->SetAttenuationParameters(1.0f, firework.attenuation_linear_start,
                                                   firework.attenuation_quadratic_start);
}

void Scene1::UpdateFireworks(float deltaTime) const
{
    if (fireworks_.empty())
    {
        return;
    }

    std::uniform_real_distribution<float> flickerNoise(-0.10f, 0.10f);

    for (auto& star : fireworks_)
    {
        if (!star || !star->light_actor)
        {
            continue;
        }

        /** Update the Age and check the percentage along its overall lifetime (0 -> 1) **/
        star->age_seconds += deltaTime;
        const float normalizedAge = std::clamp(star->age_seconds / star->lifetime_seconds, 0.0f, 1.0f);

        const Ref transform = star->light_actor->GetComponent<TransformComponent>();
        const Vec3 currentPosition = transform->GetPosition();

        /** Check if the Firework has exploded **/
        if (!star->has_exploded && star->age_seconds >= star->explosion_start)
        {
            star->has_exploded = true;
        }

        /** Firework goes up only if it hasnt exploded yet **/
        Vec3 nextPosition = currentPosition;
        if (!star->has_exploded)
        {
            nextPosition = currentPosition + star->velocity * deltaTime;
        }
        transform->SetTransform(nextPosition, transform->GetQuaternion(), transform->GetScale());

        /** Calculate the current intensity based on state **/
        float intensity = 0.0f;

        if (star->has_exploded)
        {
            /** Post-explosion: fade from peak back to baseline, then to zero **/
            /** Before Explosion: Time = Negative, After: Time = Positive **/
            const float timeSinceExplosion = star->age_seconds - star->explosion_start;
            /** Before Explosion: Time = Negative, After: Time = Positive **/
            const float totalTimeAfterExplosion = star->lifetime_seconds - star->explosion_start;
            const float explosionFade = std::max(0.0f, 1.0f - (timeSinceExplosion / totalTimeAfterExplosion));

            /** Spike at explosion, then decay **/
            const float explosionSpike = std::exp(-timeSinceExplosion * 1.5f);
            intensity = star->explosion_intensity_peak * explosionSpike * explosionFade;
        }
        else
        {
            /** Pre-explosion (ascent): gradually increase intensity as it rises with smooth curve **/
            const float ascentProgress = star->age_seconds / star->ascent_duration;
            /** Smoothstep: slow at start and end, fast in middle **/
            const float smoothProgress = ascentProgress * ascentProgress * (3.0f - 2.0f * ascentProgress);
            intensity = star->base_intensity * smoothProgress;

            /** Add light flicker during ascent **/
            const float flickerWave = 0.5f + 0.5f * std::sin(star->age_seconds * star->flicker_speed);
            const float randomFlicker = flickerNoise(fireworks_random_seed_);
            intensity *= (1.0f + star->flicker_amplitude * 0.15f * (flickerWave + randomFlicker - 0.5f));
        }

        /** Avoid Negative Values **/
        intensity = std::max(0.0f, intensity);

        /** Attenuation evolves over lifetime **/
        const float linearAttenuation =
            star->attenuation_linear_start +
            (star->attenuation_linear_end - star->attenuation_linear_start) * normalizedAge;
        const float quadraticAttenuation =
            star->attenuation_quadratic_start +
            (star->attenuation_quadratic_end - star->attenuation_quadratic_start) * normalizedAge;

        star->light_actor->SetLightIntensityMultiplier(intensity);
        star->light_actor->SetAttenuationParameters(1.0f, linearAttenuation, quadraticAttenuation);

        /** Respawn when lifetime expired or intensity dies out **/
        if (star->age_seconds >= star->lifetime_seconds)
        {
            /** Instead of creating another pointer we can reuse this one **/
            SpawnFirework(*star);
        }
    }
}


void Scene1::Render() const
{
    glClearColor(0.02f, 0.00f, 0.05f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    const Ref<ShaderComponent> shader = board_->GetComponent<ShaderComponent>();

    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                       camera_->GetProjectionMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera_->GetViewMatrix());
    glUniform4fv(static_cast<GLint>(shader->GetUniformID("ambientLightColor")), 1,
                 Vec4(1.0f, 0.0f, 0.5f, 0.0f));


    glUniform3fv(static_cast<GLint>(shader->GetUniformID("cameraWorldPosition")), 1,
                 camera_->GetComponent<TransformComponent>()->GetPosition());

    /** Render Point Light Models **/
    glUniform1i(
        static_cast<GLint>(shader->GetUniformID("activePointLightCount")),
        0
    );
    for (const auto& pointLightItem : all_lights_)
    {
        glUniform4fv(static_cast<GLint>(shader->GetUniformID("ambientLightColor")), 1,
                     pointLightItem.lock()->GetPointLightParameters().specularLightColor * pointLightItem.lock()->
                     GetPointLightParameters().lightIntensityMultiplier);
        glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                           pointLightItem.lock()->GetModelMatrix());
        glBindTexture(GL_TEXTURE_2D, pointLightItem.lock()->GetComponent<MaterialComponent>()->getTextureID());
        pointLightItem.lock()->GetComponent<MeshComponent>()->Render();
    }

    /** Rendering Point Lights **/
    UploadPointLightsToShader(shader, all_lights_);
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

void Scene1::ReferenceCountCheck(
    const Ref<Actor>& board,
    const std::vector<Ref<Actor>>& pieces,
    const std::vector<Ref<LightActor>>& staticLights,
    const std::vector<Ref<Firework>> fireworks,

    const std::string& sceneName,
    Check_context context)
{
    const bool isPostCreation = (context == Check_context::POST_CREATION);

    std::cout << "\n========== " << (isPostCreation ? "POST-CREATION" : "PRE-DESTRUCTION")
        << " MEMORY CHECK ==========" << '\n';
    std::cout << "Scene: " << sceneName << '\n';
    std::cout << "Context: " << (isPostCreation
                                     ? "After OnCreate() - Verifying initial state"
                                     : "Before OnDestroy() cleanup - Checking for leaks") << '\n';

    MemoryDiagnostics::PrintRefCount(board, "Board");
    std::cout << "  Board should have use_count = 1 (only scene holds it)" << '\n';

    MemoryDiagnostics::PrintRefCounts(pieces, "Chess Pieces");
    std::cout << "  Pieces should have use_count = 1 each (only in chess_piece_actors_ vector)" << '\n';

    MemoryDiagnostics::PrintRefCounts(staticLights, "Static Lights");
    std::cout << "  Lights should have use_count = 1  each (a shared pointer in point_lights, since all_lights is a weak pointer)"
        << '\n';
    
    MemoryDiagnostics::PrintRefCountsForFireworks(fireworks, "Dynamic Firework Lights");
    std::cout << "  Firework lights should have use_count = 1 each (held in fireworks vector via light_actor member)"
        << '\n';

    if (isPostCreation)
    {
        std::cout << "\n  ✓ If all use_counts are 1, scene was created correctly without circular references" <<
            '\n';
        std::cout << "  ✗ If any use_count > 1, there's already a memory issue after creation" << '\n';
    }
    else
    {
        std::cout << "\n  ✓ If all use_counts are 1, memory will be properly freed on scene destruction" << '\n';
        std::cout << "  ✗ If any use_count > 1, there's a circular reference or external holder preventing cleanup"
            << '\n';
    }

    std::cout << "========================================\n" << '\n';
}

void Scene1::UploadPointLightsToShader(
    const Ref<ShaderComponent>& shader,
    const std::vector<std::weak_ptr<LightActor>>& pointLightActorList)
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
        if (!pointLightActor.lock() || pointLightActor.lock() == nullptr)
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

        const Ref transformComponent = pointLightActor.lock()->GetComponent<TransformComponent>();
        const Vec3 pointLightWorldPosition = transformComponent->GetPosition();

        const PointLightParameters& pointLightParameters = pointLightActor.lock()->GetPointLightParameters();

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
