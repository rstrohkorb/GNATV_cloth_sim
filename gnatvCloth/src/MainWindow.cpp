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
  // starting position of corner 1
  connect(m_ui->m_cornerX, SIGNAL(valueChanged(double)), m_gl, SLOT(setCornerX(double)));
  connect(m_ui->m_cornerY, SIGNAL(valueChanged(double)), m_gl, SLOT(setCornerY(double)));
  connect(m_ui->m_cornerZ, SIGNAL(valueChanged(double)), m_gl, SLOT(setCornerZ(double)));
  // weft/warp/shear tests
  connect(m_ui->m_runWeftTest, SIGNAL(clicked()), m_gl, SLOT(runWeftTest()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
