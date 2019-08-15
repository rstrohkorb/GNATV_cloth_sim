#include <QMouseEvent>
#include <QGuiApplication>

#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/SimpleVAO.h>
#include <ngl/VAOFactory.h>
#include <ngl/Transformation.h>
#include <iostream>

#include "NGLScene.h"

NGLScene::NGLScene(QWidget *_parent) : QOpenGLWidget( _parent )
{
//  // initialize clothInterface
//  std::vector<size_t> corners = {0, 1, 2, 3};
//  m_cloth = Cloth(WOOL);
//  // lambda to get the cloth points from world space into 2D parametric space
//  auto toParam = [](ngl::Vec3 _v) -> ngl::Vec2
//  {
//      ngl::Vec2 n;
//      n.m_x = _v.m_x;
//      n.m_y = _v.m_z;
//      return n;
//  };
//  m_cloth.init("obj/clothLowResXZ.obj", toParam, corners, 9.0f);
//  std::vector<bool> fixedCorners = {1, 1, 1, 1};
//  //m_cloth.fixCorners(fixedCorners);
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
    m_timerId = _event->timerId();
//    for(size_t i = 0; i < 10; ++i)
//    {
//        m_cloth.update(0.001f, ngl::Vec3(0.0f));
//    }
    if(m_writeToFile)
    {
       m_ci.writeOutCloth();
    }
    m_ci.updateCloth(0.01f);
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
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // set the camera
  ngl::Vec3 from = {0.0f, 10.0f, 20.0f};
  m_view = ngl::lookAt(from, ngl::Vec3::zero(), ngl::Vec3::up());
  // add a light from the camera pos
  m_lightPos.set(from.m_x, from.m_y, from.m_z, 1.0f);

  // load shader instance
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  // ngl checker shader
  shader->use(ngl::nglCheckerShader);
  shader->setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);
  shader->setUniform("checkOn",true);
  shader->setUniform("lightPos",m_lightPos.toVec3());
  shader->setUniform("colour1",0.9f,0.9f,0.9f,1.0f);
  shader->setUniform("colour2",0.6f,0.6f,0.6f,1.0f);
  shader->setUniform("checkSize",15.0f);

  //make a simple vao for the cloth triangles
  m_clothVAO = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // wireframe
  if(m_wireframe)
  {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  // determine mouse rotation
  ngl::Mat4 rotx;
  ngl::Mat4 roty;
  ngl::Mat4 mouseRotation;
  rotx.rotateX(m_win.spinXFace);
  roty.rotateY(m_win.spinYFace);
  mouseRotation = roty * rotx;

  // render cloth
  std::vector<float> tri;
  m_ci.renderCloth(tri);

  m_clothVAO->bind();
  m_clothVAO->setData(ngl::SimpleVAO::VertexData(tri.size()*sizeof(float), tri[0]));

  m_clothVAO->setVertexAttributePointer(0, 3, GL_FLOAT, 8*sizeof(float), 0);
  m_clothVAO->setVertexAttributePointer(1, 3, GL_FLOAT, 8*sizeof(float), 3);
  m_clothVAO->setVertexAttributePointer(2, 2, GL_FLOAT, 8*sizeof(float), 6);
  m_clothVAO->setNumIndices((tri.size()/8)*3);

  loadMatrixToCheckerShader(mouseRotation);

  m_clothVAO->draw();
  m_clothVAO->unbind();
}

void NGLScene::loadMatrixToCheckerShader(const ngl::Mat4 &_tx)
{
    ngl::ShaderLib* shader = ngl::ShaderLib::instance();
    shader->use(ngl::nglCheckerShader);
    ngl::Mat4 MVP= m_project * m_view * _tx;
    ngl::Mat3 normalMatrix= m_view * _tx;
    normalMatrix.inverse().transpose();
    shader->setUniform("MVP",MVP);
    shader->setUniform("normalMatrix",normalMatrix);
}

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
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

void NGLScene::toggleWireframe(bool _mode)
{
    m_wireframe=_mode;
    update();
}

void NGLScene::startSim()
{
    startTimer(10);
}

void NGLScene::stopSim()
{
    killTimer(m_timerId);
}

void NGLScene::resetSim()
{
    killTimer(m_timerId);
    m_ci.initCloth();
    m_ci.fixClothPts();
    update();
}

void NGLScene::changeConfig(int _config)
{
    // select config
    switch(_config)
    {
    case 0: m_ci.setConfig(LRXZ); break;
    case 1: m_ci.setConfig(LRXY); break;
    case 2: m_ci.setConfig(HRXZ); break;
    case 3: m_ci.setConfig(HRXY); break;
    default: break;
    }
    // redo fixpts, since reinit occured
    m_ci.fixClothPts();
    // redraw
    update();
}

void NGLScene::changeFixpt(int _fixpt)
{
    switch(_fixpt)
    {
    case 0: m_ci.setFixPtSetup(NONE); break;
    case 1: m_ci.setFixPtSetup(CORNERS); break;
    case 2: m_ci.setFixPtSetup(HANG); break;
    case 3: m_ci.setFixPtSetup(FLAG); break;
    case 4: m_ci.setFixPtSetup(WEFT_TEST); break;
    case 5: m_ci.setFixPtSetup(WARP_TEST); break;
    default: break;
    }
}

void NGLScene::changeIntMethod(int _intm)
{
    switch(_intm)
    {
    case 0: m_ci.setIntMethod(CGM); break;
    case 1: m_ci.setIntMethod(RK4); break;
    default: break;
    }
}

void NGLScene::toggleWind(bool _isWindOn)
{
    m_ci.setWindState(_isWindOn);
}

void NGLScene::toggleWriteOut(bool _writeOut)
{
    m_writeToFile = _writeOut;
}

void NGLScene::setCornerX(double _x)
{
    m_ci.setClothPtPos(1, ngl::Vec3(static_cast<float>(_x), 0.0f, 0.0f));
    update();
}

void NGLScene::setCornerY(double _y)
{
    m_ci.setClothPtPos(1, ngl::Vec3(0.0f, static_cast<float>(_y), 0.0f));
    update();
}

void NGLScene::setCornerZ(double _z)
{
    m_ci.setClothPtPos(1, ngl::Vec3(0.0f, 0.0f, static_cast<float>(_z)));
    update();
}

void NGLScene::reinitClothToGraphs()
{
    m_ci.reinitClothToGraphs();
}

void NGLScene::runWeftTest()
{
    m_ci.runWeftTest();
}
