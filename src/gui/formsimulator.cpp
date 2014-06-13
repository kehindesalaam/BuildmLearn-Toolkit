#include "gui/formsimulator.h"

#include "gui/formmain.h"
#include "definitions/definitions.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/application.h"
#include "miscellaneous/skinfactory.h"
#include "core/templatesimulator.h"

#include <QWidget>
#include <QCloseEvent>


FormSimulator::FormSimulator(FormMain* parent)
  : QDialog(parent), m_ui(new Ui::FormSimulator), m_mainWindow(parent), m_activeSimulation(NULL) {
  m_ui->setupUi(this);
  m_isSticked = qApp->settings()->value(APP_CFG_SIMULATOR, "is_sticked", true).toBool();

  // Do necessary initializations.
  setupIcons();
  setupPhoneWidget();

  connect(parent, SIGNAL(moved()), this, SLOT(conditionallyAttachToParent()));
  connect(parent, SIGNAL(resized()), this, SLOT(conditionallyAttachToParent()));

  // This window mustn't be deleted when closed by user.
  setAttribute(Qt::WA_DeleteOnClose, false);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

FormSimulator::~FormSimulator() {
  delete m_ui;
}

void FormSimulator::setActiveSimulation(TemplateSimulator *simulation) {
  if (m_activeSimulation != NULL) {
    m_activeSimulation->close();
    m_activeSimulation->deleteLater();
    m_activeSimulation = NULL;
  }

  m_activeSimulation = simulation;
  m_activeSimulation->setParent(m_ui->m_phoneWidget);
  m_activeSimulation->setGeometry(SIMULATOR_CONTENTS_OFFSET_X, SIMULATOR_CONTENTS_OFFSET_Y,
                       SIMULATOR_CONTENTS_WIDTH, SIMULATOR_CONTENTS_HEIGHT);
  m_activeSimulation->setStyleSheet("QWidget { border: 3px solid red; }");
  m_activeSimulation->show();
}

void FormSimulator::conditionallyAttachToParent() {
  if (m_isSticked) {
    attachToParent();
  }
}

void FormSimulator::setIsSticked(bool is_sticked) {
  if (is_sticked && !m_isSticked) {
    attachToParent();
  }
  else if (!is_sticked && m_isSticked) {
    unAttachFromParent();
  }

  m_isSticked = is_sticked;

  qApp->settings()->setValue(APP_CFG_SIMULATOR, "is_sticked", is_sticked);
}

void FormSimulator::show() {
  conditionallyAttachToParent();
  QDialog::show();
}

void FormSimulator::attachToParent() {
  QPoint main_window_position = m_mainWindow->pos();
  QSize main_window_size = m_mainWindow->size();

  setFixedHeight(main_window_size.height());
  resize(size().width(), main_window_size.height());
  move(main_window_position.x() + main_window_size.width() + SIMULATOR_OFFSET,
       main_window_position.y());
}

void FormSimulator::unAttachFromParent() {
  QPoint main_window_position = m_mainWindow->pos();
  QSize main_window_size = m_mainWindow->size();

  setMinimumHeight(SIMULATOR_HEIGHT_MIN);
  setMaximumHeight(SIMULATOR_HEIGHT_MAX);

  move(main_window_position.x() + main_window_size.width() + SIMULATOR_WIDTH_OFFSET,
       main_window_position.y());
}

void FormSimulator::setupPhoneWidget() {
  m_ui->m_phoneWidget->setFixedSize(SIMULATOR_WIDTH, SIMULATOR_HEIGHT_DEFAULT);
  m_ui->m_phoneWidget->setPixmap(QPixmap(qApp->skinFactory()->currentSkin().m_simulatorBackgroundMain));

  setFixedWidth(SIMULATOR_WIDTH + SIMULATOR_WIDTH_OFFSET);
}

void FormSimulator::closeEvent(QCloseEvent *e) {
  emit closed();
  e->accept();
}

void FormSimulator::setupIcons() {
  IconFactory *factory = IconFactory::instance();

  setWindowIcon(factory->fromTheme("view-simulator"));
  m_ui->m_btnGoBack->setIcon(factory->fromTheme("simulation-back"));
  m_ui->m_btnRunSimulation->setIcon(factory->fromTheme("simulation-run"));
}