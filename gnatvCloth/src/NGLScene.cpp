#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/SimpleVAO.h>
#include <ngl/VAOFactory.h>
#include <ngl/Transformation.h>
#include <iostream>

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("GNATV Cloth Sim");
  // initialize cloth
  //std::vector<size_t> corners = {0, 29, 870, 899};
  std::vector<size_t> corners = {0, 9, 90, 99};
  m_cloth = Cloth(WOOL);
  m_cloth.init("obj/cloth.obj", XZ, corners);
  std::vector<bool> fixedCorners = {1, 1, 1, 1};
  m_cloth.fixCorners(fixedCorners);
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
    for(size_t i = 0; i < 10; ++i)
    {
        m_cloth.update(0.001f, ngl::Vec3(0.0f));
    }
    update();
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  //field of view, aspect ratio, near clipping plane, far clipping plane
  m_project = ngl::perspective(45.0f, static_cast<float>(_w/_h),
                             0.5f, 200.0f);
}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(0.6f, 0.6f, 0.6f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // load shaders
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->loadShader("ColorShader", "shaders/ColorVertex.glsl",
                   "shaders/ColorFragment.glsl");
  shader->createShaderProgram("PBR");
  shader->attachShader("PBRVertex", ngl::ShaderType::VERTEX);
  shader->attachShader("PBRFragment", ngl::ShaderType::FRAGMENT);
  shader->loadShaderSource("PBRVertex", "shaders/PBRVertex.glsl");
  shader->loadShaderSource("PBRFragment", "shaders/PBRFragment.glsl");
  shader->compileShader("PBRVertex");
  shader->compileShader("PBRFragment");
  shader->attachShaderToProgram("PBR", "PBRVertex");
  shader->attachShaderToProgram("PBR", "PBRFragment");

  // set up PBR shader to accept initial vals
  shader->linkProgramObject("PBR");
  ( *shader )["PBR"]->use();
  // set the camera
  ngl::Vec3 from = {0.0f, 20.0f, 20.0f};
  m_view = ngl::lookAt(from, ngl::Vec3::zero(), ngl::Vec3::up());
  // give camera to shader
  shader->setUniform("camPos", from);
  // add a light from the camera pos
  m_lightPos.set(from.m_x, from.m_y, from.m_z, 1.0f);
  // set uniform values in the PBR shader that can remain static
  shader->setUniform("lightPosition",m_lightPos.toVec3());
  shader->setUniform("lightColor",400.0f,400.0f,400.0f);
  shader->setUniform("exposure",2.2f);
  shader->setUniform("albedo",0.950f, 0.71f, 0.29f);

  shader->setUniform("metallic",0.01f);
  shader->setUniform("roughness",0.78f);
  shader->setUniform("ao",0.2f);

  //make a simple vao for the cloth triangles
  m_clothVAO = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // determine mouse rotation
  ngl::Mat4 rotx;
  ngl::Mat4 roty;
  ngl::Mat4 mouseRotation;
  rotx.rotateX(m_win.spinXFace);
  roty.rotateY(m_win.spinYFace);
  mouseRotation = roty * rotx;

  // render cloth
  std::vector<ngl::Vec3> tri;
  m_cloth.render(tri);

  m_clothVAO->bind();
  m_clothVAO->setData(ngl::SimpleVAO::VertexData(tri.size()*sizeof(ngl::Vec3),
                                          tri[0].m_x));
  m_clothVAO->setVertexAttributePointer(0, 3, GL_FLOAT, 2*sizeof(ngl::Vec3), 0);
  m_clothVAO->setVertexAttributePointer(1, 3, GL_FLOAT, 2*sizeof(ngl::Vec3), 3);
  m_clothVAO->setNumIndices(tri.size()/2);

  loadMatrixToPBRShader(mouseRotation);
  m_clothVAO->draw();
  m_clothVAO->unbind();
}

void NGLScene::loadMatrixToPBRShader(const ngl::Mat4 &_tx)
{
    ngl::ShaderLib* shader = ngl::ShaderLib::instance();
    shader->use("PBR");
    // transform struct to match what's in shader
    struct transform
    {
      ngl::Mat4 MVP;
      ngl::Mat4 normalMatrix;
      ngl::Mat4 M;
    };
    // set transform vals
    transform t;
    t.M=m_view*_tx;
    t.MVP=m_project*t.M;
    t.normalMatrix=t.M;
    t.normalMatrix.inverse().transpose();
    // set uniforms
    shader->setUniformBuffer("TransformUBO",sizeof(transform),&t.MVP.m_00);
}

void NGLScene::loadMatrixToColorShader(const ngl::Mat4 &_tx, const ngl::Vec4 &_color)
{
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    shader->use("ColorShader");
    shader->setUniform("MVP", m_project * m_view * _tx);
    shader->setUniform("vertColor", _color);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());

  break;
  case Qt::Key_P :
      startTimer(5);
      break;
  case Qt::Key_U : m_cloth.update(0.5f, ngl::Vec3(0.0f)); break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}
