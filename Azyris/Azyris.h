#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <iostream>
#include <cstdint>

#include <core/Window.h>
#include <core/Input.h>
#include <core/Gamepad.h>

#include <buffers/buffer.h>

#include <Utils/Timer.h>
#include <Utils/Profiler.h>
#include <Utils/Helpers.h>
#include <Utils/Shapes.h>

#include <graphics/Shader.h>
#include <graphics/Texture.h>
#include <graphics/Camera2D.h>
#include <graphics/Camera3D.h>
#include <graphics/DEBUG_Renderer.h>
#include <graphics/ImGUI_Layer.h>
#include <graphics/Renderer.h>

#include <Model/Model.h>

// Vendors

#include <Bullet/btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>