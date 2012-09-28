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
#include <QMimeData>
#include <QDrag>
#include <QDesktopWidget>
#include <QGraphicsLinearLayout>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QGraphicsSceneDragDropEvent>
#include <QDropEvent>
//#include <QProcess>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>

#include <KDirModel>
#include <KDirLister>
#include <KFileItemDelegate>
#include <kfileplacesmodel.h>
#include <kfilepreviewgenerator.h>
#include <KGlobalSettings>
#include <KWindowSystem>
#include <KRun>
#include <KFileItem>
#include <konq_operations.h>

#include <limits.h>

#ifdef Q_OS_WIN
#  define _WIN32_WINNT 0x0500 // require NT 5.0 (win 2k pro)
#  include <windows.h>
#endif // Q_OS_WIN

#include <Plasma/Corona>
#include <Plasma/WindowEffects>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>

#include "dirlister.h"
#include "proxymodel.h"

#include "directory.h"
#include "imageprovider.h"
#include "previewgenerator.h"
#include "iconwidget.h"
#include "viewer.h"

K_EXPORT_PLASMA_APPLET(stackfolder, StackFolder)

StackFolder::StackFolder(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
      m_graphicsWidget(0),
      m_placesModel(0),
      m_iconWidget(0)
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(false);

    m_dirModel = new KDirModel(this);

    m_model = new ProxyModel(this);
    m_model->setSourceModel(m_dirModel);
    m_model->setSortLocaleAware(true);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_selectionModel = new QItemSelectionModel(m_model, this);

    if (args.count() > 0) {
        m_url = KUrl(args.value(0).toString());
    }

    setPopupIcon(KIcon("folder"));
}

void StackFolder::init()
{
    connect(KGlobalSettings::self(), SIGNAL(kdisplayFontChanged()), SLOT(fontSettingsChanged()));
    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(themeChanged()));
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
}

QColor StackFolder::textColor() const
{
    if (m_textColor != Qt::transparent) {
        return m_textColor;
    }

    return Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
}

QGraphicsWidget *StackFolder::graphicsWidget()
{
    if (m_graphicsWidget) {
        return m_graphicsWidget;
    }

    setAcceptDrops(true);

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
    connect(m_directory, SIGNAL(activatedDragAndDrop(const KFileItem&)), this, SLOT(activatedDragAndDrop(const KFileItem&)));

    qmlRegisterType<File>("File", 1, 0, "File");

    QDeclarativeEngine *m_engine = new QDeclarativeEngine;
    m_engine->addImageProvider("type", typeImageProvider);
    m_engine->addImageProvider("mode", modeImageProvider);
    m_engine->addImageProvider("preview", previewImageProvider);
    m_engine->rootContext()->setContextProperty("directory", m_directory);
    QDeclarativeComponent component(m_engine, QUrl::fromLocalFile("/usr/share/apps/plasma/plasmoids/stackfolder/contents/ui/main.qml"));
    QObject *object = component.create();
    QGraphicsLayoutItem *graphicsObject = qobject_cast<QGraphicsLayoutItem*>(object);

    QObject::connect(object, SIGNAL(currentChanged()), m_viewer, SLOT(stop()));

    m_layout = new QGraphicsLinearLayout(Qt::Vertical);
    m_layout->setContentsMargins(10, 0, 10, 0);
    m_layout->addItem(graphicsObject);


    m_graphicsWidget = new QGraphicsWidget(this);
    m_graphicsWidget->setLayout(m_layout);

    QGraphicsLinearLayout *lay = dynamic_cast<QGraphicsLinearLayout *>(layout());
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    return m_graphicsWidget;
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
	qDebug() << "StackFolder::folderChanged() >>  m_folderChanging = " <<  m_folderChanging << "m_firstChangings = " << m_firstChangings;

	if (isPopupShowing() && m_firstChangings > 1) {
	    m_needShow = true;
	    hidePopup();
	}

        if (m_downloadUrl.isParentOf(url))
	    m_model->sort(KDirModel::ModifiedTime);
	else
	    m_model->sort(KDirModel::Name);

	const int count = m_dirModel->dirLister()->itemsForDir(url).count();
	QSize size = sizeToFitIcons(count);

  	m_graphicsWidget->resize(size);
  	m_graphicsWidget->setMinimumSize(size);
  	m_graphicsWidget->setMaximumSize(size);

	if (m_firstChangings == 1)
	    m_firstChangings = 2;
	//m_folderChanging = false;
	qDebug() << "StackFolder::folderChanged() <<  m_folderChanging = " <<  m_folderChanging << "m_firstChangings = " << m_firstChangings;

    }
    updateIconWidget();
}

void StackFolder::fontSettingsChanged()
{
//    QFont font = Plasma::Theme::defaultTheme()->font(Plasma::Theme::DesktopFont);
}

void StackFolder::themeChanged()
{
/*
    if (m_textColor != Qt::transparent) {
        return;
    }
*/
}

void StackFolder::refreshIcons()
{
    m_dirModel->dirLister()->updateDirectory(m_url);
}

void StackFolder::updateIconWidget()
{
    if (!m_placesModel) {
	m_placesModel = new KFilePlacesModel(this);

        const QModelIndex index = m_placesModel->closestItem(m_topUrl);
	const KUrl url = m_placesModel->url(index);

	KFileItem item = m_dirModel->itemForIndex(QModelIndex());

        if (!item.isNull() && item.iconName() != "inode-directory") {
	    m_icon = KIcon(item.iconName(), 0, item.overlays());
        } else if (m_topUrl.protocol() == "desktop") {
	    m_icon = KIcon("user-desktop");
        } else if (m_topUrl.protocol() == "trash") {
	    m_icon = m_model->rowCount() > 0 ? KIcon("user-trash-full") : KIcon("user-trash");
        } else if (index.isValid() && url.equals(m_topUrl, KUrl::CompareWithoutTrailingSlash)) {
	    m_icon = m_placesModel->icon(index);
        } else {
	    m_icon = KIcon("folder");
        }

    //setPopupIcon(m_icon);
    setPopupIcon(NULL);

    m_iconWidget = new IconWidget(m_icon, "", this);
    connect(m_iconWidget, SIGNAL(droppedItem(QGraphicsSceneDragDropEvent*)), SLOT(iconWidgetDroppedItem(QGraphicsSceneDragDropEvent*)));
    connect(m_iconWidget, SIGNAL(clicked()), SLOT(iconWidgetClicked()));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(3, 0, 3, 6);
    layout->setSpacing(0);
    layout->setOrientation(Qt::Horizontal);
    layout->addItem(m_iconWidget);
    layout->setAlignment(m_iconWidget, Qt::AlignCenter);
    setLayout(layout);

    // Update the tooltip
    Plasma::ToolTipContent data;
    data.setMainText(i18n("Stack Folder"));
    data.setSubText(m_topUrl.fileName(KUrl::IgnoreTrailingSlash));
    data.setImage(m_icon);
    Plasma::ToolTipManager::self()->setContent(this, data);

    }
}

void StackFolder::iconWidgetClicked()
{
    qDebug() << "StackFolder::::iconWidgetClicked(): m_firstChangings=" << m_firstChangings;

    if (isPopupShowing()) {
        hidePopup();
    } else {
	//qDebug() << "StackFolder::::iconWidgetClicked(): m_firstChangings=" << m_firstChangings;
	if (!m_firstChangings) {
	    m_firstChangings = 1;
	    m_directory->setUrl(m_topUrl);
	}
        showPopup();
	//qDebug() << "StackFolder::iconWidgetClicked hasFocus=" << m_dialog->hasFocus();
	//m_dialog->setFocus();
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
		m_iconWidget->startAnimation();
		return;
	    }
	}
    }
}

void StackFolder::fileActivated()
{
    hidePopup();
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
    if (!view()) {
        return;
    }

    QSize margin = QSize(12, 12); //style()->pixelMetric(QStyle::PM_LayoutLeftMargin);

    QSize s = m_graphicsWidget->size().toSize() + margin;
    QPoint pos = popupPosition(s);
    //qDebug() << "runViewer x=" << pos.x() << " y=" << pos.y();

    m_viewer->run(path, x + pos.x(), y + pos.y(), width, height);
}

void StackFolder::iconWidgetDroppedItem(QGraphicsSceneDragDropEvent *event)
{
    const QString appletMimeType = static_cast<Plasma::Corona*>(scene())->appletMimeType();
    if (event->mimeData()->hasFormat(appletMimeType)) {
        event->ignore();
        return;
    }

    QDropEvent ev(event->screenPos(), event->dropAction(), event->mimeData(),
                  event->buttons(), event->modifiers());
    KonqOperations::doDrop(m_dirModel->dirLister()->rootItem(), m_dirModel->dirLister()->url(),
                           &ev, event->widget());
}

void StackFolder::activatedDragAndDrop(const KFileItem &item)
{

    QMimeData *mime = new QMimeData;
    QList<QUrl> urls;
    urls.append(item.url());
    mime->setUrls(urls);
    QDrag *drag = new QDrag(view());
    drag->setMimeData(mime);
    QString local_path = item.localPath();
    PreviewGenerator *gen = PreviewGenerator::createInstance();
    Q_ASSERT(gen != 0);

    if (gen->hasPreviewPixmap(local_path)) {
        drag->setPixmap(gen->getPreviewPixmap(item.localPath()).scaled(KIconLoader::SizeMedium, KIconLoader::SizeMedium, Qt::KeepAspectRatio));
    }
    else
        drag->setPixmap(item.pixmap(0));
    drag->exec(Qt::CopyAction | Qt::MoveAction | Qt::LinkAction, Qt::CopyAction);
}

void StackFolder::popupEvent(bool show)
{
    if (show) {
	//Dialog shown;
    }
    else {
	//Dialog hidden;
	if (m_needShow) {
	    m_delayedShowTimer.start(200, this);
	    m_needShow = false;
	}
	m_viewer->stop();
	m_directory->emitDialogHidden();

	if (!m_folderChanging) {
    	    m_firstChangings = 0;
	}
    }
    Plasma::PopupApplet::popupEvent(show);
}

bool StackFolder::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        return false;
    }
    return Plasma::PopupApplet::eventFilter(watched, event);
}

void StackFolder::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_delayedShowTimer.timerId()) {
        m_delayedShowTimer.stop();
	showPopup();
    }
    Plasma::PopupApplet::timerEvent(event);
}

void StackFolder::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
    	event->accept();
        if (m_viewer && m_viewer->isRun()) {
    	    m_viewer->stop();
    	}
    	else {
	    hidePopup();
    	}
    }
    m_directory->emitKeyPressed(event);

    Plasma::PopupApplet::keyPressEvent(event);
}

#include "stackfolder.moc"
