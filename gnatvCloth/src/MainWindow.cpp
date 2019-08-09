/**
 * @file MainWindow.cpp
 * @brief signals and slots for GUI
 * @author Rachel Strohkorb
*/

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_gl=new  NGLScene(this);

  // SIGNALS AND SLOTS
  m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);
  connect(m_ui->m_wireframe, SIGNAL(toggled(bool)), m_gl, SLOT(toggleWireframe(bool)));
  connect(m_ui->m_isWindOn, SIGNAL(toggled(bool)), m_gl, SLOT(toggleWind(bool)));
  // start/stop/reset sim slots
  connect(m_ui->m_startButton, SIGNAL(clicked()), m_gl, SLOT(startSim()));
  connect(m_ui->m_stopButton, SIGNAL(clicked()), m_gl, SLOT(stopSim()));
  connect(m_ui->m_resetButton, SIGNAL(clicked()), m_gl, SLOT(resetSim()));
  // combo boxes to select config
  connect(m_ui->m_configSelect, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(changeConfig(int)));
  connect(m_ui->m_fixptSelect, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(changeFixpt(int)));
  connect(m_ui->m_intMethodSelect, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(changeIntMethod(int)));
//  // set the rotation signals
//  connect(m_ui->m_rotationX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXRotation(double)));
//  connect(m_ui->m_rotationY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYRotation(double)));
//  connect(m_ui->m_rotationZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZRotation(double)));
//  /// set the scale signals
//  connect(m_ui->m_scaleX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXScale(double)));
//  connect(m_ui->m_scaleY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYScale(double)));
//  connect(m_ui->m_scaleZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZScale(double)));
//  /// set the position signals
//  connect(m_ui->m_positionX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXPosition(double)));
//  connect(m_ui->m_positionY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYPosition(double)));
//  connect(m_ui->m_positionZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZPosition(double)));
//  /// set the combo box index change signal
//  connect(m_ui->m_objectSelection,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(setObjectMode(int)));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
