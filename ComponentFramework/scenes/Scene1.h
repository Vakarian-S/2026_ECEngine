#pragma once
#include <array>
#include <unordered_map>

#include "../Actor.h"
#include "../Scene.h"
#include "../CameraActor.h"
#include "../MeshComponent.h"
#include "../actors/LightActor.h"

enum class chess_pieces: uint8_t
{
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
};

class Scene1 : public Scene
{
private:
    CameraActor* camera;
    Actor* board;
    Actor* pawn;

    Vec3 lightHeight = Vec3(0.0, 0.0, 0.0);
    bool goingUp = true;

    /** Light **/
    LightActor* pointLight;

    /** Since we do not need any gameplay logic, no need to differentiate pieces **/
    std::vector<Actor*> pieces;

    /** Meshes **/
    std::unordered_map<chess_pieces, std::shared_ptr<MeshComponent>> chessPieceMeshes;


    std::vector<Actor*> whitePieces;
    std::vector<Actor*> blackPieces;
    std::array<std::array<int, 8>, 8> boardPositions{};

public:
    Scene1();
    void MoveActorBy(const Actor* actor, const MATH::Vec3& delta);
    std::vector<int> GetColPositionListByPiece(chess_pieces pieceName);
    const char* GetMeshNameByPiece(chess_pieces pieceName);
    MATH::Vec3 GetRelativeTransformOnBoard(int row, int col);
    ~Scene1() override = default;
    bool OnCreate() override;
    void OnDestroy() override;
    void HandleEvents(const SDL_Event& sdlEvent) override;
    void Update(float deltaTime) override;
    void Render() const override;
};
