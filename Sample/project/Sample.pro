TARGET = Sample
TEMPLATE = app
QT += core
QT += gui
QT += network
QT += opengl
QT += widgets
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

SOURCES += ../../Bx2D/addon/BxAddOn_GIF.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_HQX.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_JPG.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_MD5.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_TTF.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_ZIP.cpp
SOURCES += ../../Bx2D/core/BxCoreForQT5.cpp
SOURCES += ../../Bx2D/core/BxCoreImpl.cpp
SOURCES += ../../Bx2D/skin/BxIME.cpp
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
HEADERS += ../../Bx2D/lib/BxVarVector.hpp
HEADERS += ../../Bx2D/lib/BxWebContent.hpp
HEADERS += ../../Bx2D/lib/BxXml.hpp

ASSETS_CONFIG.files += ../resource/assets/config/default.key
ASSETS_CONFIG.path = /assets/config
INSTALLS += ASSETS_CONFIG

ASSETS_SYS.files += ../../Bx2D/assets/sys/bspace.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/bspace_bright.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/button.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/button_clicked.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/done.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/done_bright.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/english.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/exit_base.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/exit_focus.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/exit_push.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_grid.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_grid_d.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_hub.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_hub_d.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_log.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_log_d.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_snap.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_snap_d.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_start.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/icon_start_d.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/korean.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/min_base.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/min_focus.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/min_push.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/shift.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/shift_selected.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/sysbutton.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/sysbutton_clicked.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/text_grid.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/text_hub.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/text_log.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/text_snap.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/text_start.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/tip_left.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/tip_mid.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/tip_right.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/title.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/win_frame.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/win_play.png
ASSETS_SYS.files += ../../Bx2D/assets/sys/hoonr.ttf
ASSETS_SYS.files += ../../Bx2D/assets/sys/huy152.ttf
ASSETS_SYS.files += ../../Bx2D/assets/sys/nanum_gothic_extra_bold.ttf
ASSETS_SYS.path = /assets/sys
INSTALLS += ASSETS_SYS

android{
	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../resource/android
}

ios{
	QMAKE_BUNDLE_DATA += ASSETS_CONFIG
	QMAKE_BUNDLE_DATA += ASSETS_SYS
}

macx{
        QMAKE_BUNDLE_DATA += ASSETS_CONFIG
        QMAKE_BUNDLE_DATA += ASSETS_SYS
}
