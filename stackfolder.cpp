/*
 *   Copyright © 2011 ROSA Lab. <support@rosalab.ru>
 *   Copyright © 2008, 2009 Fredrik Höglund <fredrik@kde.org>
 *   Copyright © 2008 Rafael Fernández López <ereslibre@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#include "stackfolder.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsLinearLayout>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeItem>
//#include <QProcess>

#include <KDirModel>
#include <KDirLister>
#include <KFileItemDelegate>
#include <kfileplacesmodel.h>
#include <kfilepreviewgenerator.h>
#include <KGlobalSettings>
#include <KWindowSystem>
#include <KRun>

#include <limits.h>

#ifdef Q_OS_WIN
#  define _WIN32_WINNT 0x0500 // require NT 5.0 (win 2k pro)
#  include <windows.h>
#endif // Q_OS_WIN

#include <Plasma/WindowEffects>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>

#include "dirlister.h"
#include "dialog.h"
#include "proxymodel.h"

#include "directory.h"
#include "imageprovider.h"
#include "previewgenerator.h"
#include "iconwidget.h"
#include "viewer.h"

K_EXPORT_PLASMA_APPLET(stackfolder, StackFolder)

StackFolder::StackFolder(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_dialog(0),
      m_graphicsWidget(0),
      m_graphicsObject(0),
     // m_previewGenerator(0),
      m_placesModel(0),
      m_iconWidget(0)
      //m_viewerProcess(0)
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(false);

    m_dirModel = new KDirModel(this);

    m_model = new ProxyModel(this);
    m_model->setSourceModel(m_dirModel);
    m_model->setSortLocaleAware(true);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

//    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(rowsInserted(QModelIndex,int,int)));

    m_selectionModel = new QItemSelectionModel(m_model, this);

//    m_layout = 0;

    if (args.count() > 0) {
        m_url = KUrl(args.value(0).toString());
        //setUrl(KUrl(args.value(0).toString()));
    }
//qDebug() << "StackFolder::StackFolder()";
}

void StackFolder::init()
{


    // Find out about icon and font settings changes
    connect(KGlobalSettings::self(), SIGNAL(kdisplayFontChanged()), SLOT(fontSettingsChanged()));
    connect(this, SIGNAL(geometryChanged()), SLOT(iconGeometryChanged()));

    // Find out about theme changes
    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(themeChanged()));

    // Find out about network availability changes
    //connect(Solid::Networking::notifier(), SIGNAL(statusChanged(Solid::Networking::Status)),
    //        SLOT(networkStatusChanged(Solid::Networking::Status)));


    //m_customIconSize      = 64;
    //m_showPreviews        = true;
    //m_numTextLines        = 2;
    //m_textColor           = QColor(Qt::transparent);
    //m_iconsLocked         = false;
    m_previewPlugins      = QStringList() << "ffmpegthumbs" 		// Video
		  			  << "imagethumbnail" 		// Image
		  			  << "jpegthumbnail" 		// Jpeg
		  			  << "svgthumbnail" 		// Svg
		  			  << "windowsimagethumbnail" 	// Windows images
		  			  << "gsthumbnail"; 		// PostScript, PDF, DVI
	  	  			  // << "djvuthumbnail" 	// DjVu
	  	  			  // << "textthumbnail" 	// Text
		  			  // << "opendocumentthumbnail" // ODF (OpenDocument Format)
		  			  // << "exrthumbnail" 		// EXR images
		  			  // << "comicbookthumbnail" 	// Comic books
		  			  // << "rawthumbnail" 		// RAW
		  			  // << "windowsexethumbnail" 	// Microsoft Windows exec
		  			  // << "desktopthumbnail" 	// Desktop
		  			  // << "fontthumbnail" 	// Fonts
		  			  // << "htmlthumbnail" 	// HTML
		  			  // << "mobithumbnail" 	// Mobipocket
		  			  // << "webarchivethumbnail" 	// Web archives
		  			  // << "directorythumbnail" 	// Directories
    m_sortDirsFirst       = true;
    m_sortColumn          = int(KDirModel::Name);
    m_filterType          = 0;
    m_hideForChangeFolder = true;


//    m_flow = layoutDirection() == Qt::LeftToRight ? IconView::LeftToRight : IconView::RightToLeft;


    m_model->setFilterMode(ProxyModel::filterModeFromInt(m_filterType));
    m_model->setSortDirectoriesFirst(m_sortDirsFirst);
    m_model->setDynamicSortFilter(m_sortColumn != -1);
    m_model->sort(m_sortColumn != -1 ? m_sortColumn : KDirModel::Name, Qt::AscendingOrder);

    KDirLister *lister = new DirLister(this);
    lister->setDelayedMimeTypes(true);
    lister->setAutoErrorHandlingEnabled(false, 0);

    m_dirModel->setDirLister(lister);
    connect(lister, SIGNAL(completed(const KUrl&)), SLOT(folderChanged(const KUrl&)));

    if (!m_url.isValid()) {
        QString path = QDir::homePath();
        m_url = config().readEntry("url", KUrl(path));
        //setUrl(config().readEntry("url", KUrl(path)));
    } else {
        config().writeEntry("url", m_url);
    }

    m_topUrl = m_url;

    QProcess proc;
    proc.start( QString::fromLatin1("xdg-user-dir"), QStringList() << QString::fromLatin1("DOWNLOAD") );
    if (proc.waitForStarted() && proc.waitForFinished())
        m_downloadUrl = KUrl(QString::fromLocal8Bit(proc.readAll()).trimmed());
    else
	m_downloadUrl = KUrl();

    m_firstChangings = 0;
    m_folderChanging = false;
    m_needShow = false;
}

StackFolder::~StackFolder()
{
    if (m_graphicsObject)
	delete m_graphicsObject;
}

QColor StackFolder::textColor() const
{
    if (m_textColor != Qt::transparent) {
        return m_textColor;
    }

    return Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
}

void StackFolder::updateIconViewState()
{
/*
    QPalette palette = m_iconView->palette();
    palette.setColor(QPalette::Text, textColor());
    m_iconView->setPalette(palette);

    m_iconView->setDrawShadows(m_drawShadows);
    m_iconView->setIconSize(iconSize());
    m_iconView->setTextLineCount(m_numTextLines);
    m_iconView->setFlow(m_flow);
    m_iconView->setWordWrap(m_numTextLines > 1);
    m_iconView->setIconsMoveable(!m_iconsLocked);
    if (m_label) {
        m_label->setPalette(palette);
        m_label->setDrawShadow(m_drawShadows);
    }
*/
}

void StackFolder::constraintsEvent(Plasma::Constraints constraints)
{
    if (m_graphicsWidget) {
        return;
    }

    if (constraints & Plasma::FormFactorConstraint) {
        if (formFactor() != Plasma::Planar && formFactor() != Plasma::MediaCenter) {

            // Set up the icon widget
	    m_iconWidget = new IconWidget(KIcon("user-folder"), "", this);
            connect(m_iconWidget, SIGNAL(clicked()), SLOT(iconWidgetClicked()));

//            updateIconWidget();
	    updateIconViewState();

	    PreviewGenerator *previewGenerator = PreviewGenerator::createInstance();
	    previewGenerator->setPlugins(m_previewPlugins);

	    TypeImageProvider *typeImageProvider = new TypeImageProvider();
	    ModeImageProvider *modeImageProvider = new ModeImageProvider();
	    PreviewImageProvider *previewImageProvider = new PreviewImageProvider();

    	    m_viewer = new Viewer;

	    m_directory = new Directory(this);
	    m_directory->setModel(m_model);
	    m_directory->setTopUrl(m_url);
	    m_directory->setUrl(m_url);
	    connect(m_directory, SIGNAL(fileActivated()), this, SLOT(fileActivated()));
	    connect(m_directory, SIGNAL(dataAdded(const QModelIndex&, int, int)), this, SLOT(dataAdded(const QModelIndex&, int, int)));
	    connect(m_directory, SIGNAL(showRequested(const QString&, int, int, int, int)), this, SLOT(runViewer(const QString&, int, int, int, int)));

	    m_graphicsWidget = new QGraphicsWidget(this);
/*
	    m_graphicsWidget->setFocusPolicy(Qt::StrongFocus);
	    m_graphicsWidget->setFocusProxy(m_graphicsObject->graphicsItem());
	    m_graphicsWidget->setFocus();
*/

	    qmlRegisterType<File>("File", 1, 0, "File");

	    m_declView = new QDeclarativeView();
	    m_declView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
	    m_declView->setFocusPolicy(Qt::StrongFocus);
	    m_declView->setStyleSheet("background:transparent");

	    m_declView->engine()->addImageProvider("type", typeImageProvider);
	    m_declView->engine()->addImageProvider("mode", modeImageProvider);
	    m_declView->engine()->addImageProvider("preview", previewImageProvider);
	    m_declView->rootContext()->setContextProperty("directory", m_directory);
	    m_declView->setSource(QUrl::fromLocalFile("/usr/share/apps/plasma/plasmoids/stackfolder/contents/ui/main.qml"));

    	    QObject *item = m_declView->rootObject();
    	    QObject::connect(item, SIGNAL(currentChanged(/*int*/)), m_viewer, SLOT(/*currentChanged*/stop/*ViewerProcess*/(/*int*/)));

            m_dialog = new Dialog;
            m_dialog->setGraphicsWidget(m_graphicsWidget);
            m_dialog->setDeclarativeView(m_declView);
            m_dialog->setViewer(m_viewer);
	    connect(m_dialog, SIGNAL(hidden()), this, SLOT(dialogHidden()));



            QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);
            layout->addItem(m_iconWidget);
            setLayout(layout);
            setAspectRatioMode(Plasma::ConstrainedSquare);
        }
    }
}

void StackFolder::configChanged()
{
/*
    const KUrl url = m_url;
    m_url = config().readEntry("url", m_url);
    if (url != m_url) {
        setUrl(m_url);
        m_dirModel->dirLister()->openUrl(m_url);
    }
*/
}


void StackFolder::folderChanged(const KUrl& url)
{

    if (m_graphicsWidget) {

	m_folderChanging = true;

	if (!m_dialog->isHidden() && m_firstChangings > 1) {
	    m_needShow = true;
	    hideDialog();
	}
	//qDebug() << "StackFolder::folderChanged(): m_needShow=" << m_needShow;

        if (m_downloadUrl.isParentOf(url))
	    m_model->sort(KDirModel::ModifiedTime);
	    //m_model->setSortMode(ProxyModel::SortByDate);
	else
	    m_model->sort(KDirModel::Name);
	    //m_model->setSortMode(ProxyModel::SortByName);

	const int count = m_dirModel->dirLister()->itemsForDir(url).count();
	QSize size = sizeToFitIcons(count);

  	m_graphicsWidget->resize(size);
  	m_graphicsWidget->setMinimumSize(size);
  	m_graphicsWidget->setMaximumSize(size);

        if (m_needShow) {
	    m_needShow = false;
	    showDialog();
	}

	if (m_firstChangings == 1)
	    m_firstChangings = 2;
	//qDebug() << "StackFolder::folderChanged(): m_firstChangings=" << m_firstChangings;
	m_folderChanging = false;

    }
    updateIconWidget();
}

void StackFolder::fontSettingsChanged()
{
//    QFont font = Plasma::Theme::defaultTheme()->font(Plasma::Theme::DesktopFont);
}

void StackFolder::iconGeometryChanged()
{
    if (m_iconWidget) {
        //qDebug("StackFolder::iconGeometryChanged");
        m_iconWidget->update(m_iconWidget->boundingRect());
    }
}

void StackFolder::themeChanged()
{
/*
    if (m_textColor != Qt::transparent) {
        return;
    }

    if (m_iconView) {
        QPalette palette = m_iconView->palette();
        palette.setColor(QPalette::Text, textColor());
        m_iconView->setPalette(palette);
    }

    if (m_label) {
        QPalette palette = m_label->palette();
        palette.setColor(QPalette::Text, textColor());
        m_label->setPalette(palette);
    }
*/
}

void StackFolder::networkStatusChanged(Solid::Networking::Status status)
{
    if (status == Solid::Networking::Connected && !m_url.isLocalFile() &&
        m_url.protocol() != "desktop") {
        refreshIcons();
    }
}

void StackFolder::refreshIcons()
{
    m_dirModel->dirLister()->updateDirectory(m_url);
}

void StackFolder::updateIconWidget()
{
    if (!m_iconWidget) {
        return;
    }

    if (!m_placesModel) {
	m_placesModel = new KFilePlacesModel(this);

        const QModelIndex index = m_placesModel->closestItem(m_topUrl);
	const KUrl url = m_placesModel->url(index);

        KIcon icon;

	KFileItem item = m_dirModel->itemForIndex(QModelIndex());

        if (!item.isNull() && item.iconName() != "inode-directory") {
	    icon = KIcon(item.iconName(), 0, item.overlays());
        } else if (m_topUrl.protocol() == "desktop") {
	    icon = KIcon("user-desktop");
        } else if (m_topUrl.protocol() == "trash") {
	    icon = m_model->rowCount() > 0 ? KIcon("user-trash-full") : KIcon("user-trash");
        } else if (index.isValid() && url.equals(m_topUrl, KUrl::CompareWithoutTrailingSlash)) {
	    icon = m_placesModel->icon(index);
        } else {
	    icon = KIcon("folder");
        }
        m_iconWidget->setIcon(icon);
	m_iconWidget->update();

    }

    // Update the tooltip
    Plasma::ToolTipContent data;
    data.setMainText(i18n("Stack Folder"));
    data.setSubText(m_topUrl.fileName(KUrl::IgnoreTrailingSlash));
    data.setImage(m_iconWidget->icon());
    Plasma::ToolTipManager::self()->setContent(m_iconWidget, data);
}

void StackFolder::iconWidgetClicked()
{
    //Plasma::WindowEffects::slideWindow(m_dialog, location());
/*
    if (m_iconAnimationGroup->state() != QAbstractAnimation::Stopped)
	qDebug() << "StackFolder::iconWidgetClicked(): animation isn't stopped";
    m_iconAnimationGroup->stop();
*/
    if (m_dialog->isVisible()) {
        hideDialog();
    } else {
	//qDebug() << "StackFolder::::iconWidgetClicked(): m_firstChangings=" << m_firstChangings;

	if (!m_firstChangings) {
	    m_firstChangings = 1;
	    m_directory->setUrl(m_topUrl);
	}
	showDialog();
	//qDebug() << "StackFolder::iconWidgetClicked hasFocus=" << m_dialog->hasFocus();
	m_dialog->setFocus();
	//m_declView->setFocus();
    }
}

void StackFolder::dataAdded(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i < end + 1; i++) {
	const QModelIndex index = m_model->index(i, 0);
	const KFileItem item = m_model->itemForIndex(index);
	if (item.isFile()) {
	    const KFileItem parentItem = m_model->itemForIndex(parent);
	    if (m_downloadUrl.isParentOf(parentItem.url())) {
	    //if (parentItem.iconName().contains("folder-downloads")) {
		m_iconWidget->startAnimation();
		return;
	    }
	}
    }
}


void StackFolder::fileActivated()
{
    hideDialog();
}

void StackFolder::dialogHidden()
{
/*    if (m_needShow) {
    	    m_delayedShowTimer.start(200, this);
	else
	    m_delayedShowTimer.start(100, this);
	m_needShow = false;
    }
    else {
*/
    m_viewer->stop();

    if (!m_folderChanging)
    	if (!m_directory->isTopUrl()) {
	    //withoutHiding = true;
	    m_directory->setUrl(m_topUrl);
	}
	m_directory->emitDialogHidden();
//    }
}

void StackFolder::showDialog()
{
    Plasma::WindowEffects::slideWindow(m_dialog, location());
    m_dialog->show(this);
}

void StackFolder::hideDialog()
{
    Plasma::WindowEffects::slideWindow(m_dialog, location());
    m_dialog->hide();
}

QSize StackFolder::iconSize() const
{
    const int defaultSize = KIconLoader::global()->currentSize( KIconLoader::Panel);
    const int size = (m_customIconSize != 0) ? m_customIconSize : defaultSize;
    return QSize(size, size);
}

QSize StackFolder::sizeToFitIcons(const int count) const
{
    const int spacing = 10;
    const int margin  = 10;

    QSize labelSize = QSize(120, 20);

    const QSize maxSize = QApplication::desktop()->availableGeometry().size();
    const int  maxRows = (maxSize.height() * 0.8 - 60 - 2 * margin) / 120;
    const int maxCols = (maxSize.width() * 0.8 - 2 * margin)/ 120;
    //qDebug() << " maxWidth:" << maxSize.width() << " maxHeight:" << maxSize.height();
    int rows = sqrt(count);
    int cols = rows--;

    if (rows <= 0)
  		rows = 1;
    if (cols <= 0)
  		cols = 1;
  	if (rows > maxRows)
  		rows = maxRows;
  	if (cols > maxCols)
  		cols = maxCols;
    while (((rows == 1 && cols < 4) || (rows == 2 && cols < 5) || (rows > 2 && cols < 2 * rows)) && cols <  maxCols && rows * cols < count)
    	cols++;
    while (rows <  maxRows && rows * cols < count)
    	rows++;


  	QSize viewSize = QSize(cols*120, rows*120);
  	QSize size = QSize(viewSize.width() + 2 * margin + 40,
  			   labelSize.height() + viewSize.height() + 2 * margin + spacing);

    return size;
}

void StackFolder::runViewer(const QString &path, int x, int y, int width, int height)
{
    m_viewer->run(path, x + m_dialog->x(), y + m_dialog->y(), width, height);
}

bool StackFolder::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        return false;
    }
    return Applet::eventFilter(watched, event);
}

void StackFolder::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_delayedShowTimer.timerId()) {
        m_delayedShowTimer.stop();
	showDialog();
    }
    Applet::timerEvent(event);
}

QSizeF StackFolder::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{

    if (which == Qt::PreferredSize) {
        if (!constraint.isEmpty()) {
            return QSizeF(400, 400).boundedTo(constraint);
        } else {
            return QSizeF(400, 400);
        }
    }

    return Applet::sizeHint(which, constraint);
}

#include "stackfolder.moc"