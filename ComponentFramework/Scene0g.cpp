#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene0g.h"
#include <MMath.h>

#include "CameraActor.h"
#include "Debug.h"

#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "TransformComponent.h"

void Scene0g::MoveActorBy(const MATH::Vec3& delta)
{
    Ref transform = actor->GetComponent<TransformComponent>();
    const Vec3 position = transform->GetPosition();

    transform->SetTransform(
        Vec3(position.x + delta.x, position.y + delta.y, position.z + delta.z),
        transform->GetQuaternion()
    );
}

Scene0g::Scene0g() :
    drawInWireMode{false}
{
    Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0g::~Scene0g()
{
    Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene0g::OnCreate()
{
    camera = new CameraActor(nullptr, 45.0f, 16.0f / 9.0f, 0.5f, 1000.0f);
    camera->AddComponent<TransformComponent>(nullptr, Vec3(1.5f, -1.0f, -10.0f), Quaternion());
    camera->OnCreate();
    
    actor = new Actor(nullptr);
    actor->AddComponent<MeshComponent>(nullptr, "meshes/Mario.obj");
    actor->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    actor->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.0f, 0.0f), Quaternion(0.0f, Vec3(0.0f, 1.0f, 0.0f)));
    actor->AddComponent<MaterialComponent>(nullptr, "textures/mario_main.png");
    actor->OnCreate();
    actor->ListComponents();

    plane = new Actor(nullptr);
    plane->AddComponent<MeshComponent>(nullptr, "meshes/Plane.obj");
    plane->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
    plane->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.5f, 0.0f),
                                            QMath::angleAxisRotation(
                                                -90.0f, Vec3(1.0f, 0.0f, 0.0f)),
                                            Vec3(1.0f, 1.0f, 1.0f));
    plane->AddComponent<MaterialComponent>(nullptr, "textures/8x8_checkered_board.png");
    plane->OnCreate();
    plane->ListComponents();


    /** Gamepad Setup **/
    int gamepadCount = 0;
    SDL_JoystickID* gamepadIds = SDL_GetGamepads(&gamepadCount);
    if (gamepadIds && gamepadCount > 0)
    {
        gamepad = SDL_OpenGamepad(gamepadIds[0]);
        if (gamepad)
        {
            SDL_Log("Opened gamepad: %s", SDL_GetGamepadName(gamepad));
        }
    }
    SDL_free(gamepadIds);

    return true;
}

void Scene0g::OnDestroy()
{
    if (gamepad)
    {
        SDL_CloseGamepad(gamepad);
        gamepad = nullptr;
    }
}

void Scene0g::HandleEvents(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.type)
    {
    case SDL_EVENT_KEY_DOWN:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_A: MoveActorBy(Vec3(-0.25f, 0.0f, 0.0f));
            break;
        case SDL_SCANCODE_D: MoveActorBy(Vec3(0.25f, 0.0f, 0.0f));
            break;
        case SDL_SCANCODE_W: MoveActorBy(Vec3(0.0f, 0.0f, -0.25f));
            break;
        case SDL_SCANCODE_S: MoveActorBy(Vec3(0.0f, 0.0f, 0.25f));
            break;
        default: break;
        }
        break;

    case SDL_EVENT_GAMEPAD_ADDED:
        if (!gamepad)
        {
            SDL_JoystickID id = sdlEvent.gdevice.which;
            gamepad = SDL_OpenGamepad(id);
            if (gamepad)
            {
                SDL_Log("Gamepad added: %s", SDL_GetGamepadName(gamepad));
            }
        }
        break;

    case SDL_EVENT_GAMEPAD_REMOVED:
        if (gamepad)
        {
            SDL_JoystickID id = sdlEvent.gdevice.which;
            // Verify the removed one matches what we opened
            SDL_Joystick* joy = SDL_GetGamepadJoystick(gamepad);
            if (joy && SDL_GetJoystickID(joy) == id)
            {
                SDL_Log("Gamepad removed");
                SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
            }
        }
        break;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        switch (sdlEvent.gbutton.button)
        {
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT: MoveActorBy(Vec3(-0.25f, 0.0f, 0.0f));
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: MoveActorBy(Vec3(0.25f, 0.0f, 0.0f));
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_UP: MoveActorBy(Vec3(0.0f, 0.0f, -0.25f));
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN: MoveActorBy(Vec3(0.0f, 0.0f, 0.25f));
            break;
        default: break;
        }
        break;


    case SDL_EVENT_MOUSE_MOTION:
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        break;

    default:
        break;
    }
}

static float NormalizeAxisValue(Sint16 axisValue)
{
    return (axisValue >= 0)
               ? static_cast<float>(axisValue) / 32767.0f
               : static_cast<float>(axisValue) / 32768.0f;
}


void Scene0g::Update(const float deltaTime)
{
    if (!gamepad) return;

    const Sint16 rawX = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    const Sint16 rawY = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

    constexpr float deadZone = 8000.0f;
    /* 16-bit integer (Sint16) = 32768 */


    float x = (fabsf(static_cast<float>(rawX)) > deadZone) ? static_cast<float>(rawX) / 32768.0f : 0.0f;
    float y = (fabsf(static_cast<float>(rawY)) > deadZone) ? static_cast<float>(rawY) / 32768.0f : 0.0f;

    const float lengthSquared = x * x + y * y;
    if (lengthSquared > 1.0f)
    {
        const float inverseLength = 1.0f / sqrtf(lengthSquared);
        x *= inverseLength;
        y *= inverseLength;
    }

    constexpr float moveSpeed = 3.0f;
    const Vec3 delta(
        x * moveSpeed * deltaTime,
        0.0f,
        y * moveSpeed * deltaTime
    );

    if (delta.x != 0.0f || delta.z != 0.0f)
    {
        MoveActorBy(delta);
    }

    const float rightX = NormalizeAxisValue(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX));
    const float rightY = NormalizeAxisValue(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY));

    constexpr float rotationDeadZone = 0.15f;
    float yawInput = (fabsf(rightX) > rotationDeadZone) ? rightX : 0.0f;
    float pitchInput = (fabsf(rightY) > rotationDeadZone) ? rightY : 0.0f;

    // Often you want up on stick to pitch up
    pitchInput = -pitchInput;

    if (yawInput != 0.0f || pitchInput != 0.0f)
    {
        constexpr float yawSpeedDegreesPerSecond = 180.0f;
        constexpr float pitchSpeedDegreesPerSecond = 180.0f;

        const float yawDeltaDegrees = yawInput * yawSpeedDegreesPerSecond * deltaTime;
        const float pitchDeltaDegrees = pitchInput * pitchSpeedDegreesPerSecond * deltaTime;

        Ref transform = actor->GetComponent<TransformComponent>();
        const Quaternion currentRotation = transform->GetQuaternion();

        const Quaternion yawDelta = QMath::angleAxisRotation(yawDeltaDegrees, Vec3(0.0f, 1.0f, 0.0f));

        // Local right axis for pitch: rotate (1,0,0) by current rotation.
        // If your math lib has a method to rotate vectors by quaternions, use it.
        // Otherwise pitch around world X as a quick test.
        Vec3 localRightAxis(1.0f, 0.0f, 0.0f);

        const Quaternion pitchDelta = QMath::angleAxisRotation(pitchDeltaDegrees, localRightAxis);

        const Quaternion newRotation = yawDelta * currentRotation * pitchDelta;

        transform->SetTransform(transform->GetPosition(), newRotation);
        actor->OnCreate();
    }
}

void Scene0g::Render() const
{
    glClearColor(0.12f, 0.12f, 0.12f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Ref shader = actor->GetComponent<ShaderComponent>();

    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                       camera->GetProjectionMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera->GetViewMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                       actor->GetComponent<TransformComponent>()->GetTransformMatrix());
    glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
    actor->GetComponent<MeshComponent>()->Render();

    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1,GL_FALSE,
                       plane->GetModelMatrix());
    glBindTexture(GL_TEXTURE_2D, plane->GetComponent<MaterialComponent>()->getTextureID());
    plane->GetComponent<MeshComponent>()->Render();

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
