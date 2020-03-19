#ifndef VP_CAMERA_HPP
#define VP_CAMERA_HPP

#include <glm/gtc/matrix_transform.hpp>

class VPCamera
{
public:

  VPCamera() :
    near(0.1f),
    far(10.0f),
    fieldOfView(glm::radians(45.0f)),
    aspectRatio(1.0f),
    position(glm::vec3(0.0f)),
    forward(glm::vec3(0.0f, 1.0f, 0.0f)),
    up(glm::vec3(0.0f, 0.0f, 1.0f))
  {
    init();
  };

  VPCamera(glm::vec3& _pos,  glm::vec3& _forward, glm::vec3& _up,
           float      _near, float      _far,     float      _fov, float _aspectRatio)
  :
    near(_near),
    far(_far),
    fieldOfView(glm::radians(_fov)),
    aspectRatio(_aspectRatio),
    position(_pos),
    forward(_forward),
    up(_up)
  {
    init();
  };

  ~VPCamera() {};

  inline void setNear(float _near, bool _shouldUpdateProj = true)
  {
    near = _near;
    if (_shouldUpdateProj) updateProjection();
  }

  inline void setFar(float _far, bool _shouldUpdateProj = true)
  {
    far = _far;
    if (_shouldUpdateProj) updateProjection();
  }

  inline void setFoV(float _fov, bool _shouldUpdateProj = true)
  {
    fieldOfView = glm::radians(_fov);
    if (_shouldUpdateProj) updateProjection();
  }

  inline void setAspectRatio(float _aspectRatio, bool _shouldUpdateProj = true)
  {
    aspectRatio = _aspectRatio;
    if (_shouldUpdateProj) updateProjection();
  }

  inline void setPosition(glm::vec3 _newPos, bool _shouldUpdateView = true)
  {
    position = _newPos;

    if (_shouldUpdateView)
      view = glm::lookAt(position, position + forward, up);
  }

  inline void setForward(glm::vec3 _newForward, bool _shouldUpdateView = true)
  {
    forward = _newForward;

    if (_shouldUpdateView)
      view = glm::lookAt(position, position + forward, up);
  }

  inline void setUp(glm::vec3 _newUp, bool _shouldUpdateView = true)
  {
    up = _newUp;

    if (_shouldUpdateView)
      view = glm::lookAt(position, position + forward, up);
  }

  inline void translate(const glm::vec3& _translation)
  {
    position += glm::normalize(glm::cross(forward, up)) * _translation.x +
                forward * _translation.y +
                up * _translation.z;

    view      = glm::lookAt(position, position + forward, up);
  }
  // TODO: Gimball Lock safe rotation

  inline glm::mat4 getProjMat()  const { return projection; }
  inline glm::mat4 getViewMat()  const { return view; }

private:

  float     near;
  float     far;
  float     fieldOfView;
  float     aspectRatio;
  glm::vec3 position;
  glm::vec3 forward;
  glm::vec3 up;
  glm::mat4 view;
  glm::mat4 projection;

  inline void init()
  {
    view = glm::lookAt(position, position + forward, up);
    updateProjection();
  }

  inline void updateProjection()
  {
    projection = glm::perspective(fieldOfView, aspectRatio, near, far);

    // GLM was designed for OpenGL, that has the Y coordinate of the clip coordinates inverted
    projection[1][1] *= -1;
  }
};

#endif