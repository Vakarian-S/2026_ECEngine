#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Actor.h"
#include "CameraActor.h"


/// Forward declarations 
union SDL_Event;


class Scene0g : public Scene {
private:
	CameraActor* camera;
	Actor * actor;
	Actor * plane;
	SDL_Gamepad* gamepad = nullptr;
	void MoveActorBy(const MATH::Vec3& delta);
	bool drawInWireMode;

public:
	explicit Scene0g();
	virtual ~Scene0g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE0_H