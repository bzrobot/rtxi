/*
 * Copyright (C) 2004 Boston University
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <debug.h>
#include <main_window.h>
#include <mutex.h>
#include <plugin.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qworkspace.h>
#include <rtxi_config.h>
#include <settings.h>
#include <qsettings.h>
#include <algorithm>
#include <qaction.h>
#include <stdlib.h>
#include <qstringlist.h>
#include <qtextstream.h>

MainWindow::MainWindow(void) :
  QMainWindow(NULL, NULL, Qt::WType_TopLevel)
{
  /* Initialize Window Settings */
  setCaption("RTXI - Real-time eXperimental Interface");

  /* Insert Menu Items */
  int id;
  fileMenu = new QPopupMenu(this);
  id = fileMenu->insertItem("&Load Workspace", this, SLOT(loadSettings()), CTRL
      + Key_L);
  fileMenu->setWhatsThis(id, "Load Workspace settings file");
  id = fileMenu->insertItem("&Save Workspace", this, SLOT(saveSettings()), CTRL
      + Key_S);
  fileMenu->setWhatsThis(id, "Save Workspace settings file");
  fileMenu->insertSeparator();
  id = fileMenu->insertItem("&Quit", qApp, SLOT(closeAllWindows()), CTRL
      + Key_Q);
  fileMenu->setWhatsThis(id, "Quits the Application");
  menuBar()->insertItem("&File", fileMenu);
  fileMenu->insertSeparator();

  moduleMenu = new QPopupMenu(this);
  menuBar()->insertItem("&Modules", moduleMenu);

  utilMenu = new QPopupMenu(this);
  menuBar()->insertItem("&Utilities", utilMenu);

  updateUtilModules();


  systemMenu = new QPopupMenu(this);
  menuBar()->insertItem("&System", systemMenu);

  windowsMenu = new QPopupMenu(this);
  windowsMenu->setCheckable(true);
  QObject::connect(windowsMenu,SIGNAL(aboutToShow(void)),this,SLOT(windowsMenuAboutToShow(void)));
  menuBar()->insertItem("&Windows", windowsMenu);
  menuBar()->insertSeparator();
  QPopupMenu *helpMenu = new QPopupMenu(this);
  id = helpMenu->insertItem("What's &This", this, SLOT(whatsThis()), SHIFT
      + Key_F1);
  helpMenu->insertSeparator();
  helpMenu->setWhatsThis(id,
      "Allows you to click on any module to get a description of it");
  id = helpMenu->insertItem("&About RTXI", this, SLOT(about()));
  helpMenu->setWhatsThis(id, "Opens a Window Containing Information About RTXI");
  id = helpMenu->insertItem("About &COMEDI", this, SLOT(aboutComedi()));
  helpMenu->setWhatsThis(id,
      "Opens a Window Containing Information About COMEDI");
  id = helpMenu->insertItem("About &Qt", this, SLOT(aboutQt()));
  helpMenu->setWhatsThis(id,
      "Opens a Window Containing Information About the Qt Widget Toolkit");
  menuBar()->insertItem("&Help", helpMenu);

  /* Create and Initialize the Workspace */
  setCentralWidget(new QWorkspace(this, NULL));
  ((QWorkspace *) centralWidget())->setScrollBarsEnabled(true);

}

MainWindow::~MainWindow(void)
{
}

int
MainWindow::createFileMenuItem(const std::string &text,
    const QObject *receiver, const char *member)
{
  return fileMenu->insertItem(text, receiver, member);
}

void
MainWindow::setFileMenuItemParameter(int menuid, int parameter)
{
  fileMenu->setItemParameter(menuid, parameter);
}

void
MainWindow::clearFileMenu(void)
{
  // don't clear the entire menu b/c Load & Save Workspace and Quit are created by
  // main_window.cpp, not while a module is loading
  fileMenu->clear();
  int id;
  id = fileMenu->insertItem("&Load Workspace", this, SLOT(loadSettings()), CTRL
      + Key_L);
  fileMenu->setWhatsThis(id, "Load Workspace settings file");
  id = fileMenu->insertItem("&Save Workspace", this, SLOT(saveSettings()), CTRL
      + Key_S);
  fileMenu->setWhatsThis(id, "Save Workspace settings file");
  fileMenu->insertSeparator();
  id = fileMenu->insertItem("&Quit", qApp, SLOT(closeAllWindows()), CTRL
      + Key_Q);
  fileMenu->setWhatsThis(id, "Quits the Application");
  fileMenu->insertSeparator();

}

int
MainWindow::insertModuleMenuSeparator(void)
{
  return moduleMenu->insertSeparator();
}

int
MainWindow::createModuleMenuItem(const std::string &text,
    const QObject *receiver, const char *member)
{
  return moduleMenu->insertItem(text, receiver, member);
}

void
MainWindow::setModuleMenuItemParameter(int menuid, int parameter)
{
  moduleMenu->setItemParameter(menuid, parameter);
}

void
MainWindow::clearModuleMenu(void)
{
  moduleMenu->clear();
}

void
MainWindow::changeModuleMenuItem(int id, QString text)
{
  moduleMenu->changeItem(id, text);
}

void
MainWindow::removeModuleMenuItem(int id)
{
  moduleMenu->removeItem(id);
}

void
MainWindow::removeModuleMenuItemAt(int id)
{
  moduleMenu->removeItemAt(id);
}

int
MainWindow::createUtilMenuItem(const std::string &text,
    const QObject *receiver, const char *member)
{
  return utilMenu->insertItem(text, receiver, member);
}

void
MainWindow::setUtilMenuItemParameter(int menuid, int parameter)
{
  utilMenu->setItemParameter(menuid, parameter);
}

void
MainWindow::clearUtilMenu(void)
{
  utilMenu->clear();
}

void
MainWindow::changeUtilMenuItem(int id, QString text)
{
  utilMenu->changeItem(id, text);
}

void
MainWindow::removeUtilMenuItem(int id)
{
  utilMenu->removeItem(id);
}

int
MainWindow::insertSystemMenuSeparator(void)
{
  return systemMenu->insertSeparator();
}

int
MainWindow::createSystemMenuItem(const std::string &text,
    const QObject *receiver, const char *member)
{
  return systemMenu->insertItem(text, receiver, member);
}

void
MainWindow::removeSystemMenuItem(int id)
{
  systemMenu->removeItem(id);
}

void
MainWindow::about(void)
{
  QMessageBox::about(this, "About RTXI", "Version " + QString(VERSION)
      + "\n\nReleased under the GPLv2.\nSee www.rtxi.org for details.");
}

void
MainWindow::aboutQt(void)
{
  QMessageBox::aboutQt(this);
}

void
MainWindow::aboutComedi(void)
{
  QString text;
  QStringList lines;
  QFile file("/proc/comedi");
  if (file.open(IO_ReadOnly))
    {
      text = "COMEDI is an open source library that provides access to DAQ"
        " cards from\na variety of manufacturers. You are currently using ";
      QTextStream stream(&file);
      QString line;
      QString comediversion = stream.readLine();
      comediversion.replace("comedi", "COMEDI");
      line = stream.readLine();
      line = stream.readLine();
      if (line == "no devices")
        line = "No DAQ cards were detected.";
      else
        line = "/dev/comedi" + line.stripWhiteSpace();
      lines += line;
      text = text + comediversion
          + ".\n\nThe following DAQ cards were detected on your system:"
            "\n\nDevice name   Driver name      Board name   # Subdevices";
    }
  else
    {
      text = "COMEDI does not seem to be installed correctly on your system.";
    }

  QMessageBox::about(
      this,
      "About COMEDI",
      QString(text) + "\n\n" + lines.join("\n")
          + "\n\nTo"
            " calibrate your DAQ card, use the terminal command 'comedi_calibrate' \nfollowed"
            " by the device name of the card:\n\n  $ sudo comedi_calibrate /dev/comedi0");

}

void
MainWindow::loadSettings(void)
{
  QSettings userprefs;
  userprefs.setPath("RTXI.org", "RTXI", QSettings::User);
  QString settingsDir = userprefs.readEntry("/dirs/setfiles", getenv("HOME"));

  QFileDialog dialog(this, 0, true);
  //dialog.setDir(".");
  dialog.setDir(settingsDir);
  dialog.setFilters(QString("Settings (*.set);;All Files (*)"));
  dialog.setCaption("Choose a Settings File");
  dialog.setMode(QFileDialog::ExistingFile);

  dialog.exec();

  QString filename = dialog.selectedFile();
  if (filename != "/")
    Settings::Manager::getInstance()->load(filename);
}

void
MainWindow::saveSettings(void)
{
  QSettings userprefs;
  userprefs.setPath("RTXI.org", "RTXI", QSettings::User);
  QString settingsDir = userprefs.readEntry("/dirs/setfiles", getenv("HOME"));

  QFileDialog dialog(this, 0, true);
  //dialog.setDir(".");
  dialog.setDir(settingsDir);
  dialog.setFilters(QString("Settings (*.set);;All Files (*)"));
  dialog.setCaption("Choose a Settings File");
  dialog.setMode(QFileDialog::AnyFile);

  dialog.exec();

  QString filename = dialog.selectedFile();
  if (filename != "/")
    {
      if ((dialog.selectedFilter() == "Settings (*.set)")
          && !filename.endsWith(".set"))
        filename += ".set";
      if (QFileInfo(filename).exists() && QMessageBox::warning(this,
          "File Exists", "Do you wish to overwrite " + filename + "?",
          QMessageBox::Yes | QMessageBox::Default, QMessageBox::No
              | QMessageBox::Escape) != QMessageBox::Yes)
        {
          DEBUG_MSG("MainWindow::saveSettings : canceled overwrite\n");
          return;
        }
      Settings::Manager::getInstance()->save(filename);
    }
}

void
MainWindow::updateUtilModules()
{
  QSettings userprefs;
  userprefs.setPath("RTXI.org", "RTXI", QSettings::User);

  QStringList entries = userprefs.entryList("/utilFileList");
  int numUtilFiles = entries.size();

  if (numUtilFiles != 5) { // no settings file exists, so write one
    userprefs.writeEntry("/utilFileList/" + QString::number(0), "neuron.so");
    userprefs.writeEntry("/utilFileList/" + QString::number(1), "siggen.so");
    userprefs.writeEntry("/utilFileList/" + QString::number(2), "noisegen.so");
    userprefs.writeEntry("/utilFileList/" + QString::number(3), "synch.so");
    userprefs.writeEntry("/utilFileList/" + QString::number(4), "wave_maker.so");
  }

  int i = 0;
  int menuID = utilMenu->insertItem("Model HH Neuron",this,SLOT(loadUtil(int)));
  setUtilMenuItemParameter(menuID, i);
  i++;
  menuID = utilMenu->insertItem("Signal Generator",this,SLOT(loadUtil(int)));
  setUtilMenuItemParameter(menuID, i);
  i++;
  menuID = utilMenu->insertItem("Noise Generator",this,SLOT(loadUtil(int)));
  setUtilMenuItemParameter(menuID, i);
  i++;
  menuID = utilMenu->insertItem("Synchronize Modules",this,SLOT(loadUtil(int)));
  setUtilMenuItemParameter(menuID, i);
  i++;
  menuID = utilMenu->insertItem("Wave Maker",this,SLOT(loadUtil(int)));
  setUtilMenuItemParameter(menuID, i);
}

void
MainWindow::loadUtil(int i)
{
  QSettings userprefs;
  userprefs.setPath("RTXI.org", "RTXI", QSettings::User);
  QString filename = userprefs.readEntry("/utilFileList/"
      + QString::number(i));
  Plugin::Manager::getInstance()->load(filename.latin1());
}


void
MainWindow::windowsMenuAboutToShow(void)
{
  windowsMenu->clear();

  QWorkspace *ws = dynamic_cast<QWorkspace *> (centralWidget());
  if (!ws)
    {
      ERROR_MSG("MainWindow::windowsMenuAboutToShow : centralWidget() not a QWorkspace?\n");
      return;
    }

  int cascadeID = windowsMenu->insertItem("&Cascade",ws,SLOT(cascade(void)));
  int tileID = windowsMenu->insertItem("&Tile",ws,SLOT(tile(void)));
  if (ws->windowList().isEmpty())
    {
      windowsMenu->setItemEnabled(cascadeID, false);
      windowsMenu->setItemEnabled(tileID, false);
    }

  windowsMenu->insertSeparator();
  QWidgetList windows = ws->windowList();
  for (size_t i = 0; i < windows.count(); ++i)
    {
      int id = windowsMenu->insertItem(windows.at(i)->caption(),this,SLOT(windowsMenuActivated(int)));
      windowsMenu->setItemParameter(id, i);
      windowsMenu->setItemChecked(id, ws->activeWindow() == windows.at(i));
    }
}

void
MainWindow::windowsMenuActivated(int id)
{
  QWorkspace *ws = dynamic_cast<QWorkspace *> (centralWidget());
  if (!ws)
    {
      ERROR_MSG("MainWindow::windowsMenuActivated : centralWidget() not a QWorkspace?\n");
      return;
    }

  QWidget *w = ws->windowList().at(id);
  if (w)
    {
      w->showNormal();
      w->setFocus();
    }
}

static Mutex mutex;
MainWindow *MainWindow::instance = 0;

MainWindow *
MainWindow::getInstance(void)
{
  if (instance)
    return instance;

  /*************************************************************************
   * Seems like alot of hoops to jump through, but static allocation isn't *
   *   thread-safe. So effort must be taken to ensure mutual exclusion.    *
   *************************************************************************/

  Mutex::Locker lock(&::mutex);
  if (!instance)
    {
      static MainWindow mainwindow;
      instance = &mainwindow;
    }
  return instance;
}
