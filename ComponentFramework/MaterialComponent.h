#pragma once
#include <glew.h>
#include <string>
#include "Component.h"
class MaterialComponent: public Component {
private:
	GLuint textureID;
	std::string filename;
	bool LoadImage(const char* filename);
public:
	MaterialComponent(WeakRef<Component> parent_,const char* filename_);
	virtual ~MaterialComponent();
	
	inline GLuint getTextureID() const { return textureID; }

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime_);
	virtual void Render()const;
};

