#pragma once
#include <array>
#include <unordered_map>
#include <random>

#include "../Actor.h"
#include "../Scene.h"
#include "../CameraActor.h"
#include "../MeshComponent.h"
#include "../actors/LightActor.h"
#include "../structs/Firework.h"
#include "../components/PhysicsComponent.h"
#include "../systems/CollisionSystem.h"

enum class Chess_pieces: uint8_t
{
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
};

enum class Check_context : uint8_t
{
    POST_CREATION,
    PRE_DESTRUCTION
};

class Scene1 : public Scene
{
private:
    /** Camera view modes **/
    enum class Camera_mode : uint8_t { FREE = 0, TOP, LEFT, RIGHT };

    /** Types of Collision available for the system **/
    enum class Collision_mode : uint8_t { NONE, SPHERE, AABB };

    /** What type of collision we are using **/
    Collision_mode collision_mode_ = Collision_mode::NONE;

    /** Whether to show collision wireframes for components **/
    bool show_collision_wireframes_ = false;

    /** Systems **/
    CollisionSystem collision_system_;

    /** Camera **/
    std::unique_ptr<CameraActor> camera_free_;
    std::unique_ptr<CameraActor> camera_top_;
    std::unique_ptr<CameraActor> camera_left_;
    std::unique_ptr<CameraActor> camera_right_;
    Camera_mode camera_mode_ = Camera_mode::FREE;

    /** Returns whichever camera is currently active **/
    CameraActor* GetActiveCamera() const;

    /** Board **/
    Ref<Actor> board_;
    Quaternion base_board_orientation_quaternion_;

    /** Light **/
    std::vector<Ref<LightActor>> point_lights_;
    std::vector<Ref<Firework>> fireworks_;
    std::vector<std::weak_ptr<LightActor>> all_lights_;
    mutable std::mt19937 fireworks_random_seed_;

    Vec3 light_height_ = Vec3(0.0, 0.0, 0.0);
    bool going_up_ = true;
    size_t static_point_light_count_ = 0;


    /** Since we do not need any gameplay logic, no need to differentiate pieces **/
    std::vector<Ref<Actor>> chess_piece_actors_;

    /**
     * Actors grouped by piece type — populated alongside chess_piece_actors_ in OnCreate.
     * Allows the ImGui launch panel to target a specific piece type and instance index.
     */
    std::unordered_map<Chess_pieces, std::vector<Ref<Actor>>> chess_piece_actors_map_;

    /** Meshes **/
    std::unordered_map<Chess_pieces, std::string> mesh_filenames_;
    std::unordered_map<Chess_pieces, Ref<MeshComponent>> chess_piece_meshes_;

    /** ImGui launch-panel persistent state **/
    int imgui_selected_piece_type_ = 0;
    int imgui_selected_piece_index_ = 0;


    /**
     * Randomizes and initializes a firework's state on spawn.
     * Overwrites default values with randomized parameters and updates the light actor.
     *
     * @param firework Firework instance to initialize for a new spawn.
     */
    void SpawnFirework(Firework& firework) const;
    /**
     * Advances all fireworks by the given time step.
     * Handles ascent/explosion phases, flicker, attenuation, and respawn on expiry.
     *
     * @param deltaTime Elapsed time in seconds since the last update.
     */
    void UpdateFireworks(float deltaTime) const;
    /**
     * Counts all the shared pointers references we have in the scene, we use this for diagnosing.
     * @param board 
     * @param pieces 
     * @param lights
     * @param fireworks 
     * @param sceneName 
     * @param context 
     */
    static void ReferenceCountCheck(
        const Ref<Actor>& board,
        const std::vector<Ref<Actor>>& pieces,
        const std::vector<Ref<LightActor>>& lights,
        std::vector<Ref<Firework>> fireworks,
        const std::string& sceneName = "Scene", Check_context context = Check_context::PRE_DESTRUCTION);
    /**
     * Uploads point light data to the shader.
     * @param shader The shader component to upload data to.
     * @param pointLightActorList The list of point light actors.
     */
    static void UploadPointLightsToShader(
        const Ref<ShaderComponent>& shader,
        const std::vector<std::weak_ptr<LightActor>>& pointLightActorList);

    /**
     * Adds a collision component to all actors using a Sphere collision detection.
     */
    void GenerateSphereCollisions();

    /**
     * Adds a collision component to all actors using a bounding box collision detection.
     */
    void GenerateAABBCollisions();

    /**
     * Removes the collision components for all the actors and disables collision wireframes and mode.
     * Also strips any PhysicsComponents that were attached by LaunchPiece.
     */
    void ClearCollisionBounds();

    /**
     * Attaches a PhysicsComponent (if not already present) to the target actor and
     * sets its velocity to a forward impulse, causing it to move each Update tick.
     *
     * @param actor The actor to launch.
     */
    void LaunchPiece(const Ref<Actor>& actor) const;

    /**
     * Integrates velocity for every actor that currently carries a PhysicsComponent,
     * then runs the collision system for the frame.
     *
     * @param deltaTime Elapsed time in seconds since the last update.
     */
    void UpdatePhysics(float deltaTime);

    /**
     * Renders the ImGui Components on the screen
     */
    void RenderImGui();
    void RenderCollisionWireframes() const;

public:
    Scene1();
    ~Scene1() override = default;


    bool OnCreate() override;
    void OnDestroy() override;
    void HandleEvents(const SDL_Event& sdlEvent) override;
    void Update(float deltaTime) override;
    void Render() const override;

public:
    /**
     * Retrieves a list of column positions associated with a specific chess piece.
     *
     * @param pieceName The name of the chess piece for which the column positions are required.
     * @return A vector of integers representing the column positions where the specified chess piece can be located.
     *         If the piece name is not recognized, an empty vector is returned.
     */
    static std::vector<int> GetColPositionListByPiece(Chess_pieces pieceName);
    /**
     * Moves the specified actor by a given delta.
     *
     * @param actor The actor to move. This should not be null and must have a TransformComponent.
     * @param delta The vector representing the positional change to apply to the actor's current position.
     */
    static void MoveActorBy(const Actor* actor, const Vec3& delta);
    /**
     * Calculates the relative transform for a specific position on the board
     * based on the given row and column.
     *
     * @param row The row index of the position on the board.
     * @param col The column index of the position on the board.
     * @return A Vec3 representing the relative transform at the specified position
     *         on the board, including the x, y, and z coordinates.
     */
    static Vec3 GetRelativeTransformOnBoard(int row, int col);
};
