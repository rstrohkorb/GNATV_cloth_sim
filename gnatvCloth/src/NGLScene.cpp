#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/SimpleVAO.h>
#include <ngl/VAOFactory.h>
#include <ngl/Transformation.h>
#include <iostream>
#include "WoolMaterial.h"

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("GNATV Cloth Sim");
  // initialize cloth
  WoolMaterial wm;
  m_cloth = Cloth(wm);
  m_cloth.init("obj/clothObject.obj", XY);
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
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
  //set the camera
  m_view = ngl::lookAt({0.0f, 20.0f, 20.0f}, ngl::Vec3::zero(), ngl::Vec3::up());

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
  m_clothVAO->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(ngl::Vec3), 0);
  m_clothVAO->setNumIndices(tri.size());
  loadMatrixToShader(mouseRotation, ngl::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
  m_clothVAO->draw();
  m_clothVAO->unbind();
}

void NGLScene::loadMatrixToShader(const ngl::Mat4 &_tx, const ngl::Vec4 &_color)
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
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}
