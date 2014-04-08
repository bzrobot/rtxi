/*
	 Copyright (C) 2011 Georgia Institute of Technology, University of Utah, Weill Cornell Medical College

	 This program is free software: you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation, either version 3 of the License, or
	 (at your option) any later version.

	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtGui>
#include <QMdiArea>

#include <cmdline.h>
#include <debug.h>
#include <algorithm>
#include <settings.h>
#include <rtxi_config.h>
#include <stdlib.h>
#include <main_window.h>
#include <mutex.h>
#include <plugin.h>

MainWindow::MainWindow (void) : QMainWindow(NULL, Qt::Window) {

	mdiArea = new QMdiArea;
	setCentralWidget(mdiArea);

	/* Initialize Window Settings */
	setWindowTitle("RTXI - Real-time eXperimental Interface");

	/* Initialize Menus */
	createFileActions();
	createFileMenu();

	createModuleMenu();

	createUtilMenu();

	createSystemMenu();

	createWindowsMenu();

	createHelpActions();
	createHelpMenu();

	//updateUtilModules ();

	//patchClampSubMenu = new QMenu (this);
	//utilMenu->insertItem (tr ("&Patch Clamp"), patchClampSubMenu);
}

MainWindow::~MainWindow (void) {
}

QAction* MainWindow::insertModuleMenuSeparator (void) {
	return moduleMenu->addSeparator();
}

QAction* MainWindow::createFileMenuItem(const QString &text, const QObject *receiver, const char *member) {
	return fileMenu->addAction(text, receiver, member);
}

void MainWindow::setFileMenuItemParameter(QAction *action, int parameter) {
	action->setData(parameter);
	//fileMenu->setData(menuid, parameter);
}

void MainWindow::clearFileMenu(void) {
	// don't clear the entire menu b/c Load & Save Workspace and Quit are created by
	// main_window.cpp, not while a module is loading
	/*fileMenu->clear();
		int id;
		id = fileMenu->insertItem ("&Load Workspace", this, SLOT (loadSettings ()), QKeySequence (Qt::CTRL + Qt::Key_L));
		fileMenu->setWhatsThis (id, "Load Workspace settings file");
		id = fileMenu->insertItem ("&Save Workspace", this, SLOT (saveSettings ()), QKeySequence (Qt::CTRL + Qt::Key_S));
		fileMenu->setWhatsThis (id, "Save Workspace settings file");
		fileMenu->addSeparator();
		id = fileMenu->insertItem ("&Quit", qApp, SLOT (closeAllWindows ()), QKeySequence (Qt::CTRL + Qt::Key_Q));
		fileMenu->setWhatsThis (id, "Quits the Application");
		fileMenu->addSeparator();*/
}

QAction* MainWindow::createModuleMenuItem(const QString &text, const QObject *receiver, const char *member) {
	printf("in the module create item func\n");
	return moduleMenu->addAction(text, receiver, member);
}

void MainWindow::setModuleMenuItemParameter (QAction *action, int parameter) {
	action->setData(parameter); //moduleMenu->setItemParameter (menuid, parameter);
}

void MainWindow::clearModuleMenu (void) {
	moduleMenu->clear();
}

void MainWindow::changeModuleMenuItem (QAction *action, QString text) {
	action->setText(text);//changeItem (id, text);
}

void MainWindow::removeModuleMenuItem (QAction *action) {
	//moduleMenu->removeItem (id);
}

void MainWindow::removeModuleMenuItemAt (int id) {
	//moduleMenu->removeItemAt (id);
}

QAction* MainWindow::createUtilMenuItem(const QString &text, const QObject * receiver, const char *member) {
	//return utilMenu->addAction (text, receiver, member);
}

void MainWindow::setUtilMenuItemParameter (QAction *action, int parameter) {
	action->setData(parameter);
	//utilMenu->setItemParameter (menuid, parameter);
}

void MainWindow::clearUtilMenu (void) {
	utilMenu->clear ();
}

void MainWindow::changeUtilMenuItem (int id, QString text) {
	//utilMenu->changeItem (id, text);
}

void MainWindow::removeUtilMenuItem (int id) {
	//utilMenu->removeItem(id);
}

void MainWindow::createFileMenu() {
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(load);
	fileMenu->addAction(save);
	fileMenu->addSeparator();
	fileMenu->addAction(quit);
}

void MainWindow::createModuleMenu() {
	moduleMenu = menuBar()->addMenu(tr("&Modules"));
}

void MainWindow::createUtilMenu() {
	utilMenu = menuBar()->addMenu(tr("&Utilities"));
	filtersSubMenu = utilMenu->addMenu(tr("&Filters"));
	signalsSubMenu = utilMenu->addMenu(tr("&Signal Generators"));
	patchClampSubMenu = utilMenu->addMenu(tr("&Patch Clamp"));
}

void MainWindow::createSystemMenu() {
	systemMenu = menuBar()->addMenu(tr("&System"));
}

void MainWindow::createWindowsMenu() {
	windowsMenu = menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu,SIGNAL(aboutToShow (void)),this,SLOT(windowsMenuAboutToShow(void)));
}

void MainWindow::createHelpMenu() {
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(artxi);
	helpMenu->addAction(aqt);
	helpMenu->addAction(axeno);
	helpMenu->addAction(adaq);
	helpMenu->addAction(acomedi);
}

void MainWindow::createFileActions() {
	load = new QAction(tr("&Load Workspace"), this);
	load->setShortcuts(QKeySequence::Open);
	load->setStatusTip(tr("Load a saved workspace"));
	connect(load, SIGNAL(triggered()), this, SLOT(loadSettings()));

	save = new QAction(tr("&Save Workspace"), this);
	save->setShortcuts(QKeySequence::Save);
	save->setStatusTip(tr("Save current workspace"));
	connect(save, SIGNAL(triggered()), this, SLOT(saveSettings()));

	quit = new QAction(tr("&Quit"), this);
	quit->setShortcut(tr("Ctrl+Q"));
	quit->setStatusTip(tr("Quit RTXI"));
	connect(quit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
}


void MainWindow::createMdi(QMdiSubWindow *subWindow){
	mdiArea->addSubWindow(subWindow);
}

void MainWindow::createHelpActions() {
	artxi = new QAction(tr("About &RTXI"),this);
	connect(artxi, SIGNAL(triggered()), this, SLOT(about()));

	aqt = new QAction(tr("About &Qt"),this);
	connect(aqt, SIGNAL(triggered()), this, SLOT(aboutQt()));

	axeno = new QAction(tr("About &Xenomai"),this);
	connect(axeno, SIGNAL(triggered()), this, SLOT(aboutXeno()));

	adaq = new QAction(tr("About &DAQ"),this);
	connect(adaq, SIGNAL(triggered()), this, SLOT(aboutDAQ()));

	acomedi = new QAction(tr("About &COMEDI"),this);
	connect(acomedi, SIGNAL(triggered()), this, SLOT(aboutComedi()));
}

void MainWindow::updateUtilModules(){
	QSettings userprefs;
	userprefs.setPath(QSettings::NativeFormat, QSettings::UserScope, "RTXI");
	userprefs.beginGroup("/utilFileList");
	QStringList entries = userprefs.childKeys();
	userprefs.endGroup();

	int numUtilFiles = entries.size();

	/*int i = 0;
		int menuID = utilMenu->insertItem("Model HH Neuron",this,SLOT(loadUtil(int)));
		setUtilMenuItemParameter(menuID, i);
		i++;
		menuID = utilMenu->insertItem("Synchronize Modules",this,SLOT(loadUtil(int)));
		setUtilMenuItemParameter(menuID, i);

		QPopupMenu *filterSubMenu = new QPopupMenu(this);
		i = 0;
		menuID = filterSubMenu->insertItem("FIR Filter (Window Method)",this,SLOT(loadFilter(int)));
		filterSubMenu->setItemParameter(menuID, i);
		i++;
		menuID = filterSubMenu->insertItem("IIR Filter",this,SLOT(loadFilter(int)));
		filterSubMenu->setItemParameter(menuID, i);

		utilMenu->insertItem(tr("&Filters"), filterSubMenu);

		QPopupMenu *signalSubMenu = new QPopupMenu(this);

		i = 0;
		menuID = signalSubMenu->insertItem("Signal Generator",this,SLOT(loadSignal(int)));
		signalSubMenu->setItemParameter(menuID, i);
		i++;
		menuID = signalSubMenu->insertItem("Noise Generator",this,SLOT(loadSignal(int)));
		signalSubMenu->setItemParameter(menuID, i);
		i++;
		menuID = signalSubMenu->insertItem("Wave Maker",this,SLOT(loadSignal(int)));
		signalSubMenu->setItemParameter(menuID, i);
		i++;
		menuID = signalSubMenu->insertItem("Mimic",this,SLOT(loadSignal(int)));
		signalSubMenu->setItemParameter(menuID, i);

		utilMenu->insertItem(tr("&Signals"), signalSubMenu);*/
}

QAction* MainWindow::createPatchClampMenuItem (const QString &text, const QObject *receiver, const char *member) {
	//return patchClampSubMenu->addAction(text, receiver, member);
}

QAction* MainWindow::createSystemMenuItem (const QString &text, const QObject *receiver, const char *member) {
	return systemMenu->addAction(text, receiver, member);
}

// VISIT TWO
void MainWindow::removeSystemMenuItem (QAction *removeMe) {
	//systemMenu->removeAction(actionList[0]);
}

void MainWindow::about(void) {
	QMessageBox::about (this, "About RTXI", "Version " + QString(VERSION)
			+	"\n\nReleased under the GPLv3.\nSee www.rtxi.org for details.");
}

void MainWindow::aboutQt (void) {
	QMessageBox::aboutQt(this);
}

void MainWindow::aboutXeno(void) {
	QMessageBox::about (this, "About Xenomai", "Version " + QString(system("xeno-config --version"))
			+	"\n\nReleased under the GPLv3.\nSee www.rtxi.org for details.");
}

void MainWindow::aboutDAQ(void) {
	QMessageBox::about (this, "About RTXI", "Version " + QString(system("lspci | awk '{print $7}'"))
			+	"\n\nReleased under the GPLv3.\nSee www.rtxi.org for details.");
}

void MainWindow::aboutComedi(void) {
	QString text;
	QStringList lines;
	QFile file ("/proc/comedi");
	bool DAQdetected = false;
	/*if (file.open (IO_ReadOnly)) {
		text = "COMEDI is an open source library that provides access to DAQ"
		" cards from\na variety of manufacturers. You are currently using ";
		QTextStream stream (&file);
		QString line;
		QString comediversion = stream.readLine ();
		comediversion.replace ("comedi", "COMEDI");
		line = stream.readLine ();
		line = stream.readLine ();
		if (line == "no devices") {
		line = "No DAQ cards were detected.";
		text = text + comediversion + ".";
		} else {
		line = "/dev/comedi" + line.trimmed();//stripWhiteSpace ();
		DAQdetected = true;
		text = text + comediversion
		+ ".\n\nThe following DAQ cards were detected on your system:"
		"\n\nDevice name   Driver name      Board name   # Subdevices";
		}
		lines += line;
		} else {
		text = "COMEDI does not seem to be installed correctly on your system.";
		}
		QString cmd;
		int status;
		QMessageBox notice ("RTXI COMEDI Calibration",
		"RTXI will attempt to calibrate your DAQ device on /dev/comedi0. Please wait for the results.\n",
		QMessageBox::Information,
		QMessageBox::Ok | QMessageBox::Default,
		QMessageBox::NoButton, QMessageBox::NoButton, this);
	//notice.setModal(false);
	if (DAQdetected) {
	switch (QMessageBox::information (this, "About COMEDI", QString (text)
	+ "\n\n" + lines.join ("\n")
	+
	"\n\nDo you want to calibrate your DAQ card?",
	QMessageBox::Yes, QMessageBox::No,
	QMessageBox::NoButton)) {
	case QMessageBox::Yes:
	cmd
	=
	QString
	("sudo comedi_calibrate --reset --dump --calibrate --results --verbose /dev/comedi0");
	DEBUG_MSG
	("RTXI is about to calibrate DAQ card for COMEDI driver%s\n",
	cmd.toAscii ());
	rt_printf ("calibrating DAQ card...\n");
	notice.exec ();

	status = CmdLine::getInstance ()->execute (cmd.toAscii ());

	if (status != 0) {
	ERROR_MSG ("RTXI COMEDI calibration error\n");
	notice.close ();
	QMessageBox::information (this, "RTXI COMEDI Calibration",
	"RTXI failed to calibrate your DAQ device on /dev/comedi0.\n");
	} else {
	notice.close ();
	QMessageBox::information (this, "RTXI COMEDI Calibration",
	"RTXI successfully calibrated your DAQ device on /dev/comedi0.\n");
	}
	case QMessageBox::No:
	default:		// just for sanity
	break;
	}
	} else {
	QMessageBox::information (this, "About COMEDI", QString (text) + "\n\n"
	+ lines.join ("\n"), QMessageBox::Ok);
	}*/
}

void MainWindow::loadSettings (void) {
	QSettings userprefs;
	userprefs.setPath (QSettings::NativeFormat, QSettings::UserScope, "RTXI");
	QString settingsDir = userprefs.value("/dirs/setfiles", getenv("HOME")).toString();

	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load saved workspace"), "/home/", tr("Settings (*.set)"));

	if (filename != "/")
		Settings::Manager::getInstance()->load(filename);
}

void MainWindow::saveSettings(void) {
	QSettings userprefs;
	userprefs.setPath (QSettings::NativeFormat, QSettings::UserScope, "RTXI");
	QString settingsDir = userprefs.value("/dirs/setfiles", getenv("HOME")).toString();

	QString filename = QFileDialog::getSaveFileName(this,
			tr("Save current workspace"), "/home/", tr("Settings (*.set)"));

	if (filename != "/") {
		if (!filename.endsWith(".set"))
			filename = filename+".set";
		if (QFileInfo (filename).exists() && QMessageBox::warning(this,
					"File Exists", "Do you wish to overwrite " + filename + "?",
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::No | QMessageBox::Escape) != QMessageBox::Yes) {
			DEBUG_MSG ("MainWindow::saveSettings : canceled overwrite\n");
			return;
		}
		Settings::Manager::getInstance()->save(filename);
	}
}

/*void MainWindow::updateUtilModules () {
	QSettings userprefs;
	userprefs.setPath (QSettings::NativeFormat, QSettings::UserScope, "RTXI");

	QStringList entries = userprefs.entryList ("/utilFileList");
	int numUtilFiles = entries.size ();

	for (int i = 0; i < numUtilFiles; i++) {
	userprefs.removeEntry ("/utilFileList/util" + QString::number (i));
	userprefs.removeEntry ("/utilFileList/filter" + QString::number (i));
	userprefs.removeEntry ("/utilFileList/sig" + QString::number (i));
	}
	userprefs.writeEntry ("/utilFileList/util" + QString::number (0), "neuron.so");
	userprefs.writeEntry ("/utilFileList/util" + QString::number (1), "synch.so");

	userprefs.writeEntry ("/utilFileList/filter" + QString::number (0), "FIRwindow.so");
	userprefs.writeEntry ("/utilFileList/filter" + QString::number (1), "IIRfilter.so");

	userprefs.writeEntry ("/utilFileList/sig" + QString::number (0), "siggen.so");
	userprefs.writeEntry ("/utilFileList/sig" + QString::number (1), "noisegen.so");
	userprefs.writeEntry ("/utilFileList/sig" + QString::number (2), "wave_maker.so");
	userprefs.writeEntry ("/utilFileList/sig" + QString::number (3), "mimic.so");

	int i = 0;
	int menuID = utilMenu->insertItem("Model HH Neuron", this, SLOT (loadUtil (int)));
	setUtilMenuItemParameter(menuID, i);
	i++;
	menuID = utilMenu->insertItem("Synchronize Modules", this, SLOT (loadUtil (int)));
	setUtilMenuItemParameter(menuID, i);

	QMenu *filterSubMenu = new QMenu(this);
	i = 0;
	menuID = filterSubMenu->insertItem("FIR Filter (Window Method)", this, SLOT (loadFilter (int)));
	filterSubMenu->setItemParameter(menuID, i);
	i++;
	menuID = filterSubMenu->insertItem("IIR Filter", this, SLOT (loadFilter (int)));
	filterSubMenu->setItemParameter(menuID, i);

	utilMenu->insertItem(tr("&Filters"), filterSubMenu);

	QMenu *signalSubMenu = new QMenu(this);

	i = 0;
	menuID = signalSubMenu->insertItem("Signal Generator", this, SLOT (loadSignal (int)));
	signalSubMenu->setItemParameter(menuID, i);
	i++;
	menuID = signalSubMenu->insertItem("Noise Generator", this, SLOT (loadSignal (int)));
	signalSubMenu->setItemParameter(menuID, i);
	i++;
	menuID = signalSubMenu->insertItem("Wave Maker", this, SLOT (loadSignal (int)));
	signalSubMenu->setItemParameter(menuID, i);
	i++;
	menuID = signalSubMenu->insertItem("Mimic", this, SLOT (loadSignal (int)));
	signalSubMenu->setItemParameter(menuID, i);

	utilMenu->insertItem(tr ("&Signals"), signalSubMenu);
	}*/

void MainWindow::loadUtil(int i) {
	QSettings userprefs;
	userprefs.setPath (QSettings::NativeFormat, QSettings::UserScope, "RTXI");
	/*QString filename = userprefs.readEntry ("/utilFileList/util" + QString::number (i));
		QByteArray textData = filename.toLatin1();
		const char *text = textData.constData();
		Plugin::Manager::getInstance()->load(text);*/
}

void MainWindow::loadFilter(int i) {
	QSettings userprefs;
	userprefs.setPath (QSettings::NativeFormat, QSettings::UserScope, "RTXI");
	/*QString filename = userprefs.readEntry ("/utilFileList/filter"+ QString::number (i));
		QByteArray textData = filename.toLatin1();
		const char *text = textData.constData();
		Plugin::Manager::getInstance()->load(text);*/
}

void MainWindow::loadSignal(int i) {
	QSettings userprefs;
	userprefs.setPath (QSettings::NativeFormat, QSettings::UserScope, "RTXI");
	/*QString filename = userprefs.readEntry ("/utilFileList/sig" + QString::number (i));
		QByteArray textData = filename.toLatin1();
		const char *text = textData.constData();
		Plugin::Manager::getInstance()->load(text);*/
}

void MainWindow::windowsMenuAboutToShow (void) {

	// Clear previous entries
	windowsMenu->clear ();

	// Add default options
	windowsMenu->addAction(tr("Cascade"),mdiArea,SLOT(cascadeSubWindows()));
	windowsMenu->addAction(tr("Tile"),mdiArea,SLOT(tileSubWindows()));
	windowsMenu->addSeparator();

	// Get list of open subwindows in Mdi Area
	QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

	// Make sure it isn't empty
	if(subWindows.isEmpty()){
		return;
	}

	// Create windows list based off of what's open
	// VISIT TWO
	for(int i = 0; i < subWindows.size(); i++){
		QMdiSubWindow *child = subWindows.at(i);
		windowsMenu->addAction(child->widget()->windowTitle(),this,SLOT(windowsMenuActivated(int)));
	}
}

void MainWindow::windowsMenuActivated(int id) {

	// Get list of open subwindows in Mdi Area
	QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

	// Make sure it isn't empty
	if(subWindows.isEmpty()){
		return;
	}

	// Set active selected window
	mdiArea->setActiveSubWindow(subWindows.at(id));
}

static Mutex mutex;
MainWindow * MainWindow::instance = 0;

MainWindow * MainWindow::getInstance (void) {
	if (instance)
		return instance;

	/*************************************************************************
	 * Seems like alot of hoops to jump through, but static allocation isn't *
	 *   thread-safe. So effort must be taken to ensure mutual exclusion.    *
	 *************************************************************************/

	Mutex::Locker lock (&::mutex);
	if (!instance) {
		static MainWindow mainwindow;
		instance = &mainwindow;
	}
	return instance;
}
