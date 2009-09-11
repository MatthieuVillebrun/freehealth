macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/$${RPATH_LIBS_BIN}/
} else:linux-*{
    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var

  !CONFIG(debug, debug|release):CONFIG(LINUX_INTEGRATED){
     # rpath for interdependant libs and plugs (everyone in the same dir)
     QMAKE_RPATHDIR += \$\$ORIGIN
     # rpath application to the libs path. Here the path must be an absolute path (eg:/usr/lib/appname/).
     QMAKE_RPATHDIR += $${RPATH_LIBS_BIN}


     ## todo : this MUST be improved +++
     QMAKE_RPATHDIR += /usr/$${LIBRARY_BASENAME}/FreeMedForms
     ## end todo

   }else{
     # rpath for interdependant libs and plugs (everyone in the same dir)
     QMAKE_RPATHDIR += \$\$ORIGIN
     # rpath application to the libs path. Here the path must be a relative path to the application binary.
     QMAKE_RPATHDIR += \$\$ORIGIN/$${RPATH_LIBS_BIN}
   }

    PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")
    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${PLUGIN_RPATH}\'
    QMAKE_RPATHDIR =
}


