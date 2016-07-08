#ifndef ISCENE_H
#define ISCENE_H

#include "Camera.h"
#include "glm/glm.hpp"

class IScene {
public:
	IScene(int w, int h) : m_animate(true), width(w), height(h) {}
	
    // Load textures, initialize shaders, etc.
    virtual void initScene() = 0;

    // This is called prior to every frame.  Use this to update your animation.
    virtual void update( float t ) = 0;

    // Draw your scene.
    virtual void draw(Camera* camera) = 0;

    // Called when screen is resized
    virtual void resize(int, int) = 0;
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;

    int width, height;

    glm::mat4 model, view, projection;
};

#endif // ISCENE_H