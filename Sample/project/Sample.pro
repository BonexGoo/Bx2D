TARGET = Sample
TEMPLATE = app
QT += core
QT += gui
QT += network
QT += opengl
QT += widgets
win32: QT += bluetooth
win32: RC_ICONS += ../common/main.ico

CONFIG += c++11
CONFIG += mobility
CONFIG += warn_off
MOBILITY += systeminfo

INCLUDEPATH += ../../Bx2D/addon
INCLUDEPATH += ../../Bx2D/core
INCLUDEPATH += ../../Bx2D/lib
INCLUDEPATH += ../../Bx2D/skin

DEFINES += __BX_APPLICATION
DEFINES += __BX_OPENGL
win32: DEFINES += __BX_MEMORY
debug: DEFINES += __BX_DEBUG
macx: DEFINES += __BX_USED_LONG_SIZET
ios: DEFINES += __BX_USED_LONG_SIZET

SOURCES += ../source/HelloWorld.cpp
SOURCES += ../source/SceneSwap_A.cpp
SOURCES += ../source/SceneSwap_B.cpp
SOURCES += ../source/SceneSwap_C.cpp
SOURCES += ../source/SceneSwap_D.cpp
SOURCES += ../source/SceneSwap_E.cpp

###########################################################
# Bx2D
###########################################################
SOURCES += ../../Bx2D/addon/BxAddOn_GIF.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_HQX.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_JPG.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_MD5.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_TTF.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_ZIP.cpp
SOURCES += ../../Bx2D/core/BxCoreForQT5.cpp
SOURCES += ../../Bx2D/core/BxCoreImpl.cpp
SOURCES += ../../Bx2D/skin/BxIME.cpp
win32|macx: SOURCES += ../../Bx2D/skin/BxProjectHelper.cpp
win32|macx: SOURCES += ../../Bx2D/skin/BxSimulator.cpp
SOURCES += ../../Bx2D/skin/BxStartPage.cpp
SOURCES += ../../Bx2D/skin/ime/IMEBoard.cpp
SOURCES += ../../Bx2D/skin/ime/IMEButton.cpp
SOURCES += ../../Bx2D/skin/ime/IMEEdit.cpp
SOURCES += ../../Bx2D/skin/ime/IMESysButton.cpp
SOURCES += ../../Bx2D/skin/ime/IMETip.cpp
HEADERS += ../../Bx2D/core/BxCore.hpp
HEADERS += ../../Bx2D/core/BxCoreImpl.hpp
HEADERS += ../../Bx2D/core/BxCoreForQT5.hpp
HEADERS += ../../Bx2D/lib/BxAnimate.hpp
HEADERS += ../../Bx2D/lib/BxArgument.hpp
HEADERS += ../../Bx2D/lib/BxAutoBuffer.hpp
HEADERS += ../../Bx2D/lib/BxConsole.hpp
HEADERS += ../../Bx2D/lib/BxDraw.hpp
HEADERS += ../../Bx2D/lib/BxDynamic.hpp
HEADERS += ../../Bx2D/lib/BxExpress.hpp
HEADERS += ../../Bx2D/lib/BxImage.hpp
HEADERS += ../../Bx2D/lib/BxKeyword.hpp
HEADERS += ../../Bx2D/lib/BxMemory.hpp
HEADERS += ../../Bx2D/lib/BxPanel.hpp
HEADERS += ../../Bx2D/lib/BxPathFind.hpp
HEADERS += ../../Bx2D/lib/BxPool.hpp
HEADERS += ../../Bx2D/lib/BxScene.hpp
HEADERS += ../../Bx2D/lib/BxSingleton.hpp
HEADERS += ../../Bx2D/lib/BxString.hpp
HEADERS += ../../Bx2D/lib/BxTween.hpp
HEADERS += ../../Bx2D/lib/BxType.hpp
HEADERS += ../../Bx2D/lib/BxUtil.hpp
HEADERS += ../../Bx2D/lib/BxVar.hpp
HEADERS += ../../Bx2D/lib/BxVarMap.hpp
HEADERS += ../../Bx2D/lib/BxVarQueue.hpp
HEADERS += ../../Bx2D/lib/BxVarVector.hpp
HEADERS += ../../Bx2D/lib/BxWebContent.hpp
HEADERS += ../../Bx2D/lib/BxXml.hpp

ASSETS_CONFIG.files += ../resource/assets/config
ASSETS_CONFIG.path = /assets
INSTALLS += ASSETS_CONFIG

ASSETS_SYS.files += ../../Bx2D/assets/sys
ASSETS_SYS.path = /assets
INSTALLS += ASSETS_SYS

android{
	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../resource/android
}

ios|macx{
	QMAKE_BUNDLE_DATA += ASSETS_CONFIG
	QMAKE_BUNDLE_DATA += ASSETS_SYS
}
