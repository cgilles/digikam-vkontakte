#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2018      by Maik Qualmann, <metzpinguin at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libvkontakte_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_vkapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_getapplicationpermissionsjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_authenticationdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_jobs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_userinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_userinfojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_albuminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_albumlistjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_createalbumjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_editalbumjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_deletealbumjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photoinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photolistjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_getphotouploadserverjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photopostjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_savephotojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_uploadphotosjob.cpp

# Not used (old implementation or not ported)
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_notejob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_cidsnamesjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_messageinfo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_discussionslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_messageslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_noteinfo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_noteaddjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_allmessageslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_groupinfo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_allnoteslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_noteslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_friendlistjob.cpp

# Deprecated classes
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/deprecated/vkontakte_getvariablejob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/deprecated/vkontakte_getinfojob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/deprecated/vkontakte_grouplistjob.cpp
)

# For unit-tests and plugin
add_library(vkontaktebackend STATIC ${libvkontakte_SRCS})

target_link_libraries(vkontaktebackend
                      PRIVATE
                      digikamcore
)

# --------------------------------------------------------------------------------

set(vkontakteplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/vkalbumchooser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vkauthwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vknewalbumdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vkplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vkwindow.cpp
)

add_library(Generic_VKontakte_Plugin
            MODULE ${vkontakteplugin_SRCS})

set_target_properties(Generic_VKontakte_Plugin PROPERTIES PREFIX "")

target_link_libraries(Generic_VKontakte_Plugin
                      Digikam::digikamcore

                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Gui
                      Qt5::WebEngineWidgets

                      KF5::ConfigCore
                      KF5::KIOCore
                      KF5::I18n

                      vkontaktebackend
)

MACRO_ADD_PLUGIN_INSTALL_TARGET(Generic_VKontakte_Plugin generic)
